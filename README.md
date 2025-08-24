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

- **Epic**: ECS-15 ESP32-S3 Matrix Radar System
- **Repository**: https://github.com/Zssan12/Art-Art-Art
- **Status**: ✅ 核心功能已完成

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
