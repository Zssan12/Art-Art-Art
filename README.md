# ESP32-S3 Matrix Nearby Radar

**COMP4500 - Art-Art-Art Project**

## 项目概述

这是一个基于 ESP32-S3 的设备雷达检测系统，使用 8x8 NeoPixel 矩阵实时显示附近设备的信号强度。

## 功能特点

- 🔄 **ESP-NOW 无线通信**: 设备间直接通信，无需路由器
- 📡 **实时雷达显示**: 8x8 LED 矩阵显示附近设备
- 🎨 **信号强度可视化**: 颜色从红色(弱信号)到绿色(强信号)渐变
- 📊 **柱状图显示**: 最多同时显示 8 台设备的信号强度
- ⚡ **自动设备发现**: 实时检测和移除离线设备

## 技术栈

- **硬件**: ESP32-S3, 8x8 NeoPixel Matrix
- **通信协议**: ESP-NOW
- **开发语言**: C++ (Arduino Framework)
- **库依赖**: 
  - Adafruit_NeoPixel
  - Adafruit_NeoMatrix
  - ESP32 WiFi & ESP-NOW

## 项目结构

```
src/
├── main.c++    # 主要实现代码
└── main.ino    # Arduino IDE 兼容文件
```

## JIRA 集成

- **Epic**: ECS-15 Dev - ESP32-S3 Matrix Radar System
- **Pull Request**: #1 (已合并)
- **原始仓库**: https://github.com/Ainz-X/Art-Art-Art-
- **当前仓库**: https://github.com/Zssan12/Art-Art-Art
- **Status**: ✅ 核心功能已完成，文档已更新

## 安装和使用

1. 使用 Arduino IDE 或 PlatformIO 打开项目
2. 安装所需库依赖
3. 配置硬件连接 (Matrix Pin 14)
4. 上传代码到 ESP32-S3
5. 多台设备同时运行可看到雷达效果

## 演示效果

- 无设备时：中心蓝色闪烁点
- 有设备时：彩色柱状图显示信号强度
- 颜色变化：红色(远距离) → 绿色(近距离)

## 项目协作

### 开发历程
1. **初始版本**: 来自 Ainz-X/Art-Art-Art- 仓库
2. **Pull Request #1**: ECS-15 Dev 分支合并到 main
3. **功能增强**: 添加完整的 ESP32-S3 Matrix Radar 实现
4. **文档完善**: 更新项目说明和使用指南

### 贡献者
- **原始项目**: Ainz-X
- **当前维护**: Zssan12 (zhaixinyang)

### 如何贡献
1. Fork 本仓库
2. 创建功能分支: `git checkout -b feature/your-feature`
3. 提交更改: `git commit -m "ECS-XX: 描述你的更改"`
4. 推送分支: `git push origin feature/your-feature`
5. 创建 Pull Request
