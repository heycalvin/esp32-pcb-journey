# 🔵 KiCad 路线 —— 从这里开始

> **本路线与 `jlceda/`（嘉立创 EDA）严格区分，是另一条独立的学习路径。**  
> 适合想要**免费开源工具 + 让 AI（Codex / Claude / Cursor）帮你画板、布线、改板**的零基础新手。

---

## 为什么要单独走一条 KiCad 路线？

| 对比维度 | 嘉立创 EDA（专业版） | KiCad |
| :--- | :--- | :--- |
| 价格 | 免费（绑定立创商城） | 免费开源（GPL） |
| 平台 | Windows / macOS / 网页 | Windows / macOS / Linux |
| 工程文件格式 | 专有（闭源） | **开放的纯文本**（`.kicad_sch` / `.kicad_pcb`） |
| 脚本接口 | 有限 | **官方 Python API**（`pcbnew`） |
| 能否被 AI 驱动 | 较难 | **天然适合**（文本可读写 + 有 API） |
| 打样下单 | 一键绑嘉立创 | 导出 Gerber 后自己去任意板厂（含嘉立创） |
| 元件库 | 立创商城海量编号（Cxxxxx） | 官方符号/封装库 + 社区库 |

**一句话结论**：嘉立创路线赢在「开箱即用 + 一键下单」，KiCad 路线赢在「开放 + 可被 AI 自动化」。

---

## 🎯 学习载体（由浅入深）

1. **第 01 章：LED 最小板** —— 只画「排针 + 电阻 + LED」，半天跑通 KiCad 全流程；
2. **后续章节：ESP32 核心板** —— 加主控，用 PWM 实现呼吸灯，再进阶。

---

## 📚 教程目录

| 章节 | 主题 | 状态 |
| :--- | :--- | :---: |
| [**第 01 章：KiCad 从这里开始**](book/01-kicad-first-step-led-blink.md) | 环境、工程文件结构、LED 最小板全流程 | 🟢 已发布 |
| 第 02 章：ESP32 最小系统（规划中） | 加主控、PWM 呼吸灯 | ⚪ 待写 |

---

## 📂 本目录结构约定

```plaintext
kicad/
├── book/           # KiCad 通俗教程章节（markdown）
├── hardware/       # KiCad 工程源文件
│   ├── 01-led-minimal-board/    # LED 最小板工程
│   └── 02-esp32-core-board/     # ESP32 核心板工程（规划中）
├── docs/           # KiCad 设计规范与自检清单
└── README.md       # 本文档
```

> **约定**：教程章节写入 `book/`，工程文件放入 `hardware/<编号>_<名称>/`，规范清单放入 `docs/`。新增关卡后同步更新本目录与根 `README.md` 的导航。

---

## 🛠️ 快速开始

1. 官网下载安装 KiCad 8：<https://www.kicad.org/download/windows/>
2. 读 [第 01 章](book/01-kicad-first-step-led-blink.md)，跟着从零画一块 LED 最小板。