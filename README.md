# 🚀 ESP32 PCB Journey (ESP32 硬件设计与实践之路)

本项目记录基于 **嘉立创EDA (专业版)** 进行 ESP32 系列硬件开发与 PCB 设计的学习历程、设计规范、原理图工程与实战避坑笔记。

---

## 🎯 学习目标与路线规划

- [ ] **Phase 1: 基础起步与最小系统板 (Minimum System Board)**
  - 掌握嘉立创EDA专业版的基本操作（原理图绘制、封装库匹配、PCB 布局布线与 DRC 规则）。
  - 设计一款基础的 ESP32 / ESP32-S3 / ESP32-C3 最小系统板（Type-C 供电、LDO 稳压、自动下载电路、复位/Boot 电路、板载天线/IPEX 接口）。
- [ ] **Phase 2: 进阶外设与多功能扩展板**
  - 电源管理与电池充放电电路（如 TP4056 / 锂电池充电管理、低功耗设计）。
  - 传感器与外设接口（I2C/SPI 屏幕、温湿度传感器、WS2812 RGB、蜂鸣器、按键消抖等）。
- [ ] **Phase 3: 高频射频与多层板布局规范**
  - 4 层板叠层设计（信号层-地层-电源层-信号层）与阻抗控制匹配。
  - RF 射频区域净空（Keep-out）、天线周边避让与接地过孔阵列。
  - EMI / EMC 电磁兼容性与电源滤波去耦技巧。
- [ ] **Phase 4: 投产、贴片（SMT）与硬件验证**
  - 导出 Gerber、BOM 清单与坐标文件（Pick and Place）。
  - 嘉立创一键下单与 SMT 贴片匹配规范。
  - 上电测试与硬件 Bring-up（冒烟测试、电源轨电压测量、烧录验证）。

---

## 📂 仓库目录结构

```plaintext
esp32-pcb-journey/
├── docs/                        # 学习笔记、硬件设计规范与避坑指南
│   ├── 01-design-checklist.md   # PCB 设计自检清单（投板前必看）
│   └── 02-esp32-hardware-notes.md # ESP32 硬件设计核心要点与 Strapping 引脚
├── hardware/                    # 嘉立创EDA专业版工程源码与导出产物
│   ├── 01-esp32-minimal-system/ # 阶段一：最小系统板工程
│   └── libraries/               # 自定义原理图符号与 PCB 封装库
├── firmware/                    # 硬件验证与出厂测试固件（ESP-IDF / Arduino）
└── README.md                    # 本文档
```

---

## 🛠️ 设计工具与生产规范

| 工具 / 环节 | 选用方案 | 说明 |
| :--- | :--- | :--- |
| **EDA 工具** | 嘉立创EDA (专业版) | 支持多层板、层次原理图、3D 渲染与立创商城库直连 |
| **元器件采购** | 立创商城 (SZLCSC) | 优先选择**基础库/常用库**元器件以降低 SMT 换料费 |
| **PCB 制造** | 嘉立创 (JLCPCB) | 双层板 / 4层板免费打样或经济型工艺 |
| **测试固件** | ESP-IDF / Arduino | 编写简单的 GPIO、串口通讯、Wi-Fi/BLE 扫频固件 |

---

## 📖 参考资料与官方文档

- [乐鑫官方硬件设计指南 (ESP32 Hardware Design Guidelines)](https://docs.espressif.com/)
- [嘉立创EDA 专业版帮助文档](https://prodocs.lceda.cn/)
- [立创开源硬件平台](https://oshwhub.com/)

---

## 📝 贡献与维护

- **Author**: HeyCalvin
- **License**: MIT
