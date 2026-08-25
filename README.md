# 🚀 ESP32 PCB Journey —— 小智 AI 硬件零基础实战宝典

本项目以爆款开源项目 **“小智 AI 语音聊天机器人”** 硬件底板为实战载体，结合 **嘉立创EDA (专业版)**，专为**零基础小白**打造一套通俗易懂、层层递进的 ESP32 PCB 设计与实战通关教程。

---

## 🗺️ 零基础通关路线图 (Roadmap)

```mermaid
flowchart TD
    subgraph 阶段一：认知篇
        C1["第 01 章：认识小智 AI 的“人体器官”"]
        C2["第 02 章：电路小白的第一堂课（电压/水流/阻容）"]
    end

    subgraph 阶段二：模块拆解篇
        C3["第 03 章：耳朵系统（INMP441 与 I2S 录音）"]
        C4["第 04 章：嘴巴系统（MAX98357 功放与喇叭）"]
        C5["第 05 章：眼睛与交互（OLED 屏与拨轮按键）"]
        C6["第 06 章：能量之源（锂电充放电与开关）"]
    end

    subgraph 阶段三：PCB 设计篇
        C7["第 07 章：从图纸到现实（封装与飞线）"]
        C8["第 08 章：布局艺术（器件摆放与声学隔离）"]
        C9["第 09 章：布线实战（线宽水管论与铺地铜）"]
    end

    subgraph 阶段四：生产测试篇
        C10["第 10 章：出厂体检（DRC 规则与 3D 渲染）"]
        C11["第 11 章：投产测试（免费打样与开箱点灯）"]
    end

    C1 --> C2 --> C3 --> C4 --> C5 --> C6 --> C7 --> C8 --> C9 --> C10 --> C11
```

---

## 📚 教程在线阅读（点击章节直达）

- [📖 完整教程大纲总览 (book/README.md)](book/README.md)
- [01. 认识小智 AI 的“人体器官”](book/01-xiaozhi-anatomy.md)
- [02. 电路小白的第一堂课：原理图符号与电路直觉](book/02-schematic-and-circuit-intuition.md)
- [03. 耳朵系统：INMP441 与 I2S 数字音频输入](book/03-ears-inmp441-i2s-microphone.md)
- [04. 嘴巴系统：MAX98357 功放与喇叭驱动](book/04-mouth-max98357-amplifier-speaker.md)
- [05. 眼睛与交互：0.91寸 OLED 屏与拨轮按键](book/05-eyes-oled-display-and-switch.md)
- [06. 能量之源：锂电池充放电 2 代与电源开关](book/06-power-battery-charging-management.md)
- [07. 从图纸到现实：原理图转 PCB 与封装真相](book/07-footprint-and-ratsnest-netlist.md)
- [08. 布局艺术：小智 AI 元器件怎么摆？（声学隔离与天线禁铜）](book/08-layout-rules-and-acoustic-isolation.md)
- [09. PCB 布线实战：线宽水管论、打过孔与铺地铜](book/09-routing-track-width-via-copper-pour.md)
- [10. 出厂体检：DRC 规则检查与 3D 真实装配渲染](book/10-drc-inspection-and-3d-render.md)
- [11. 投产与测试：免费打样、Bring-up 防炸机检测与小智初次对话](book/11-ordering-bringup-and-first-talk.md)

---

## 📂 仓库工程文件结构

```plaintext
esp32-pcb-journey/
├── book/                        # 核心通俗教程章节 (第 01 ~ 11 章)
├── hardware/                    # 硬件设计工程源文件
│   └── 01-xiaozhi-ai/           # 小智 AI 嘉立创EDA专业版工程及 Gerber 打样文件
│       ├── ProPrj_AI-xiaozhi.epro # 嘉立创EDA专业版工程源文件 (双击/导入打开)
│       └── Gerber_*.GTL/...     # 嘉立创生产打样 Gerber 制造文件
├── docs/                        # 补充参考规范与设计检查清单
└── README.md                    # 本文档
```

---

## 🛠️ 设计工具与实战操作

1. **EDA 工具**：嘉立创EDA (专业版) EasyEDA Pro
2. **实战工程**：[`hardware/01-xiaozhi-ai/ProPrj_AI-xiaozhi.epro`](hardware/01-xiaozhi-ai/ProPrj_AI-xiaozhi.epro)
3. **主控方案**：ESP32-S3 (Wi-Fi + BLE + 大模型 WebSocket 音频流)
