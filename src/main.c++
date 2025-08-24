/*
 * ESP32-S3 Matrix - Nearby Radar (ESP-NOW + 8x8 NeoPixel)
 * - 每台板子定时广播心跳包
 * - 接收同伴并读取 RSSI，映射到 8 列柱状条（最多显示 8 台）
 * - 颜色随强度从红(-100)到绿(-30)渐变
 */

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <map>
#include <vector>
#include <algorithm>

// ====== 矩阵配置（按你的实测已调好） ======
#define MATRIX_PIN 14        // 如无显示，改成 48 试试
#define MW 8
#define MH 8
// 颜色顺序：你拍照呈红色 -> 说明应使用 RGB（不是默认 GRB）
#define COLOR_ORDER NEO_RGB
Adafruit_NeoMatrix matrix(
  MW, MH, MATRIX_PIN,
  // 物理首像素在板子左下角 → BOTTOM + LEFT
  NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  COLOR_ORDER + NEO_KHZ800
);

// ====== Radar/ESP-NOW 参数 ======
static const char MAGIC[4] = {'R','D','R','1'};
static const uint8_t BCAST[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

static const uint8_t  WIFI_CHANNEL   = 1;      // 所有设备一致
static const uint32_t PING_INTERVAL  = 1000;   // 心跳间隔 ms
static const uint32_t PEER_EXPIRE_MS = 10000;  // 下线时间 ms
static const float    RSSI_ALPHA     = 0.7f;   // RSSI 平滑

struct Packet { char magic[4]; uint8_t mac[6]; uint32_t seq; } __attribute__((packed));
struct PeerInfo { int rssi; unsigned long lastSeen; };

std::map<String, PeerInfo> peers;
uint8_t selfMac[6];
uint32_t seqNo = 0;
unsigned long lastPing = 0;

// ====== 小工具 ======
String macToString(const uint8_t mac[6]) {
  char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  return String(buf);
}
inline int clampRSSI(int r){ return constrain(r, -100, -30); }
uint16_t colorForRSSI(int rssi) {
  int v = clampRSSI(rssi);
  uint8_t r = map(v, -100, -30, 255, 0);
  uint8_t g = map(v, -100, -30, 0, 255);
  return matrix.Color(r, g, 0);
}
void prunePeers() {
  unsigned long now = millis();
  std::vector<String> dead;
  for (auto &kv : peers) if (now - kv.second.lastSeen > PEER_EXPIRE_MS) dead.push_back(kv.first);
  for (auto &k : dead) peers.erase(k);
}

// ====== 显示：柱状条 ======
void updateMatrix() {
  prunePeers();

  std::vector<std::pair<String, PeerInfo>> list;
  list.reserve(peers.size());
  for (auto &kv : peers) list.push_back(kv);
  std::sort(list.begin(), list.end(),
            [](const auto &a, const auto &b){ return a.second.rssi > b.second.rssi; });

  matrix.fillScreen(0);
  int cols = std::min(8, (int)list.size());
  for (int i = 0; i < cols; ++i) {
    int rssi = list[i].second.rssi;
    int h = map(clampRSSI(rssi), -100, -30, 0, 8);   // 0~8 格
    uint16_t c = colorForRSSI(rssi);
    for (int y = 0; y < h; ++y) matrix.drawPixel(i, y, c);  // 底部是 y=0（因我们设了 BOTTOM）
  }

  // 没同伴：闪烁中心两点
  static bool blink = false;
  if (cols == 0) {
    blink = !blink;
    if (blink) {
      matrix.drawPixel(3, 3, matrix.Color(0, 0, 40));
      matrix.drawPixel(4, 4, matrix.Color(0, 0, 40));
    }
  }

  matrix.show();
}

// ====== 接收回调（支持 2.x/3.x 内核） ======
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (!info || !data || len < (int)sizeof(Packet)) return;
  const Packet *p = reinterpret_cast<const Packet*>(data);
  if (memcmp(p->magic, MAGIC, 4) != 0) return;

  String macStr = macToString(info->src_addr);
  int rssiNow = 0; if (info->rx_ctrl) rssiNow = info->rx_ctrl->rssi;

  auto it = peers.find(macStr);
  if (it == peers.end()) {
    peers[macStr] = { rssiNow, millis() };
  } else {
    int old = it->second.rssi;
    it->second.rssi = (int)(RSSI_ALPHA * old + (1.0f - RSSI_ALPHA) * rssiNow);
    it->second.lastSeen = millis();
  }
}

// ====== 发心跳 ======
void sendPing() {
  Packet pkt; memcpy(pkt.magic, MAGIC, 4); memcpy(pkt.mac, selfMac, 6); pkt.seq = ++seqNo;
  esp_now_send(BCAST, reinterpret_cast<const uint8_t*>(&pkt), sizeof(pkt));
}

// ====== 初始化 ======
void initWiFiEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) while (true) delay(1000);
  esp_now_register_recv_cb(onDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, BCAST, 6);
  peerInfo.channel = WIFI_CHANNEL;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

// ====== 入口 ======
void setup() {
  Serial.begin(115200);
  delay(200);

  matrix.begin();
  matrix.setBrightness(40);           // 亮度适中；可调 20~60
  matrix.fillScreen(0);
  // 开机自检：左下角白点 300ms，确认引脚/方向
  matrix.drawPixel(0, 0, matrix.Color(32, 32, 32));
  matrix.show();
  delay(300);
  matrix.fillScreen(0);
  matrix.show();

  WiFi.macAddress(selfMac);
  initWiFiEspNow();
}

void loop() {
  unsigned long now = millis();
  if (now - lastPing >= PING_INTERVAL) { sendPing(); lastPing = now; }
  updateMatrix();
  delay(80);
}