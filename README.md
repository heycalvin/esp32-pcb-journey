# 🚀 ESP32 PCB Journey —— 嘉立创 EDA 与 KiCad 双路线实战宝典

本项目为零基础小白打造的 **ESP32 PCB 设计通关教程**，从「看懂一块电路板」到「亲手画板、打样、上电点灯」。

为了覆盖不同学习路线，本仓库拆成**两个子目录，严格区分两条并行路线**：

```plaintext
esp32-pcb-journey/
├── projects/ # 📻 实战项目（ESP-NOW 对讲机等：面包板 MVP → PCB 产品化）
├── jlceda/   # 🟢 路线 A：嘉立创 EDA（专业版）—— 小智 AI 语音机器人底板
└── kicad/    # 🔵 路线 B：KiCad —— LED 最小板 → ESP32（AI 驱动 PCB）
```

> **两个子目录各自独立、互不交叉**：你想学「开箱即用 + 一键下单打样」走嘉立创路线；你想学「开源 + 可用 AI（Codex/Claude）驱动画板」走 KiCad 路线。
> **实战项目库**：提供完整的软硬件闭环项目，文档与固件全内聚（从面包板 MVP 到定制 PCB）。

---

## 📻 实战项目：ESP-NOW 无线对讲机 (从面包板 MVP 到定制 PCB)

基于 ESP32 与 ESP-NOW 协议打造超低延迟（<5ms）、免连路由器的无线对讲机，现已完成 **面包板 MVP 完整设计与 ESP-IDF 固件**：

- 🍞 [**面包板 MVP 详细设计与接线图 (MB-102 830孔)**](projects/walkie-talkie/docs/01-breadboard-mvp-design.md)
- 🛒 [**元器件采购清单与避坑指南 (BOM)**](projects/walkie-talkie/docs/02-bom-shopping-list.md)
- 📌 [**DOIT ESPS3-32 N16R8 开发板 44 引脚速查**](projects/walkie-talkie/docs/03-doit-esps3-pinout-guide.md)
- 📦 [**ESP-IDF 固件工程源码**](projects/walkie-talkie/firmware/)
- 👉 [**查看对讲机项目总览与通关路线**](projects/walkie-talkie/README.md)

---

## 🔵 路线 B：KiCad 路线（适合想用 AI 画板的新手）

- 免费、开源、跨平台（Windows / macOS / Linux）。
- 工程文件是**开放的纯文本格式**，内置 Python 脚本接口，天然适合让 AI（Codex、Claude、Cursor）帮你布局、布线、改板。
- 学习载体：先做一块 **LED 最小板**跑通全流程，再进阶 **ESP32 核心板**。

👉 [进入 KiCad 路线总览 (kicad/README.md)](kicad/README.md)

---

## 🟢 路线 A：嘉立创 EDA 路线（适合想快速出板的新手）

以爆款开源项目 **“小智 AI 语音聊天机器人”** 硬件底板为实战载体，结合 **嘉立创 EDA (专业版)**，零基础、层层递进、中文友好、绑定立创商城一键打样。

### 🗺️ 零基础通关路线图 (Roadmap)

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

### 📚 教程在线阅读（点击章节直达）

- [📖 完整教程大纲总览 (jlceda/book/README.md)](jlceda/book/README.md)
- [01. 认识小智 AI 的“人体器官”](jlceda/book/01-xiaozhi-anatomy.md)
- [02. 电路小白的第一堂课：原理图符号与电路直觉](jlceda/book/02-schematic-and-circuit-intuition.md)
- [03. 耳朵系统：INMP441 与 I2S 数字音频输入](jlceda/book/03-ears-inmp441-i2s-microphone.md)
- [04. 嘴巴系统：MAX98357 功放与喇叭驱动](jlceda/book/04-mouth-max98357-amplifier-speaker.md)
- [05. 眼睛与交互：0.91寸 OLED 屏与拨轮按键](jlceda/book/05-eyes-oled-display-and-switch.md)
- [06. 能量之源：锂电池充放电 2 代与电源开关](jlceda/book/06-power-battery-charging-management.md)
- [07. 从图纸到现实：原理图转 PCB 与封装真相](jlceda/book/07-footprint-and-ratsnest-netlist.md)
- [08. 布局艺术：小智 AI 元器件怎么摆？（声学隔离与天线禁铜）](jlceda/book/08-layout-rules-and-acoustic-isolation.md)
- [09. PCB 布线实战：线宽水管论、打过孔与铺地铜](jlceda/book/09-routing-track-width-via-copper-pour.md)
- [10. 出厂体检：DRC 规则检查与 3D 真实装配渲染](jlceda/book/10-drc-inspection-and-3d-render.md)
- [11. 投产与测试：免费打样、Bring-up 防炸机检测与小智初次对话](jlceda/book/11-ordering-bringup-and-first-talk.md)

---

## 📂 仓库工程文件结构

```plaintext
esp32-pcb-journey/
├── projects/                     # 📻 实战项目库
│   └── walkie-talkie/            # ESP-NOW 无线对讲机（完整闭环）
│       ├── README.md             # 对讲机项目总览、路线图与技术评估
│       ├── docs/                 # 面包板 MVP 实物设计指南、引脚手册与 BOM
│       └── firmware/             # 对讲机 ESP-IDF 原生驱动与 ESP-NOW 协议栈
├── jlceda/                       # 🟢 路线 A：嘉立创 EDA（专业版）
│   ├── book/                     # 核心通俗教程章节 (第 01 ~ 11 章)
│   ├── hardware/                 # 硬件工程源文件
│   │   └── 01-xiaozhi-ai/        # 小智 AI 嘉立创EDA专业版工程及 Gerber 打样文件
│   │       ├── ProPrj_AI-xiaozhi.epro  # 嘉立创EDA专业版工程源文件
│   │       └── Gerber_*.GTL/...  # 嘉立创生产打样 Gerber 制造文件
│   └── docs/                     # 设计自检清单与补充规范
├── kicad/                        # 🔵 路线 B：KiCad
│   ├── book/                     # KiCad 通俗教程章节
│   ├── hardware/                 # KiCad 工程源文件 (.kicad_sch / .kicad_pcb)
│   └── docs/                     # KiCad 设计规范与自检清单
└── README.md                     # 本文档（总导航）
```

---

## 🛠️ 设计工具与实战操作

**路线 A（嘉立创 EDA）**
1. **EDA 工具**：嘉立创 EDA（专业版）EasyEDA Pro
2. **实战工程**：[`jlceda/hardware/01-xiaozhi-ai/ProPrj_AI-xiaozhi.epro`](jlceda/hardware/01-xiaozhi-ai/ProPrj_AI-xiaozhi.epro)
3. **主控方案**：ESP32-S3 (Wi-Fi + BLE + 大模型 WebSocket 音频流)

**路线 B（KiCad）**
1. **EDA 工具**：KiCad 8（官网 `kicad.org` 免费下载）
2. **学习载体**：LED 最小板 → ESP32 核心板
3. **AI 驱动**：KiCad 工程为纯文本 + Python 接口，可交给 Codex / Claude 自动布局布线