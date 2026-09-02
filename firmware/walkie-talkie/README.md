# 📻 ESP32 ESP-NOW 对讲机 (Walkie-Talkie) ESP-IDF 固件

本项目基于 **ESP-IDF** 原生框架，利用 **ESP-NOW** 协议实现超低延迟的点对点/组网无线语音对讲。

---

## 🔌 硬件接线指南 (面包板搭建)

以常见的 **ESP32 (WROOM-32 / NodeMCU-32S)** 为例（ESP32-S3 用户请参考 `main/app_config.h` 引脚配置）：

### 1. INMP441 数字麦克风 (I2S 输入)
| INMP441 引脚 | ESP32 引脚 | 说明 |
| :--- | :--- | :--- |
| **VDD** | 3.3V | 供电 (切勿接 5V) |
| **GND** | GND | 共地 |
| **SD (DOUT)** | **GPIO 32** | 麦克风数字音频输出 |
| **WS (LRCK)** | **GPIO 15** | 左右声道选择时钟 |
| **SCK (BCLK)**| **GPIO 14** | 位时钟 |
| **L/R** | GND | 接地选择左声道 |

### 2. MAX98357A D类功放模块 (I2S 输出)
| MAX98357A 引脚 | ESP32 引脚 | 说明 |
| :--- | :--- | :--- |
| **VIN** | 5V (或 3.3V) | 建议接 5V 获得更大响度 |
| **GND** | GND | 共地 |
| **DIN** | **GPIO 22** | 扬声器数字音频输入 |
| **LRC** | **GPIO 25** | 左右声道时钟 |
| **BCLK** | **GPIO 26** | 位时钟 |
| **GAIN** | 悬空 | 默认 9dB 增益 |
| **SD** | 悬空 | 默认开启 |
| **+ / -** | 喇叭 | 接 8Ω 2W 或 4Ω 3W 喇叭两极 |

### 3. PTT 按键 (Push-To-Talk)
| 按键引脚 | ESP32 引脚 | 说明 |
| :--- | :--- | :--- |
| **KEY 引脚 1** | **GPIO 4** | 内部配置弱上拉 |
| **KEY 引脚 2** | GND | 按下时拉低至 0V |

---

## 🛠️ 编译与烧录

确保已激活 ESP-IDF 终端环境：

```bash
cd firmware/walkie-talkie

# 1. 设置目标芯片 (标准 ESP32 设为 esp32；S3 设为 esp32s3)
idf.py set-target esp32

# 2. 编译工程
idf.py build

# 3. 烧录并打开串口监视器 (请替换为实际串口号，如 /dev/cu.usbserial-xxx 或 COMx)
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## 🧪 分步调试与验证流程

### 阶段 1：单机硬件音频自环测试 (验证接线与音质)
1. 打开 `main/app_config.h`，将模式切换为：
   ```c
   #define CURRENT_APP_MODE  APP_MODE_LOCAL_LOOPBACK
   ```
2. 烧录单块 ESP32 并上电。
3. 对着 INMP441 麦克风说话，喇叭应立即同步输出你的声音。
4. **排查提示**：若声音小可调节 `main.c` 里的增益放大系数；若有杂音检查面包板杜邦线是否接触不良或是否共地良好。

---

### 阶段 2：双机 ESP-NOW 实时对讲
1. 打开 `main/app_config.h`，将模式恢复为：
   ```c
   #define CURRENT_APP_MODE  APP_MODE_WALKIE_TALKIE
   ```
2. 将相同的固件烧录进 **两块 ESP32** 开发板。
3. 两板均上电后：
   - **设备 A** 按住 PTT 键（GPIO 4 对地短接），LED 常亮，对着麦克风说话；
   - **设备 B** 扬声器实时同步播报，延迟通常小于 30ms；
   - 设备 A 松开 PTT 键，恢复监听模式；
   - **设备 B** 按下 PTT 键，反向对讲同理。

---

## 📈 后续进阶升级路线

1. **音频压缩编解码**：引入 **IMA-ADPCM**（将 16-bit 压缩至 4-bit，带宽降为原本的 1/4）或 **Opus** 语音编码，进一步增强抗干扰能力。
2. **多频道切换**：通过旋钮或按键切换 ESP-NOW 频道分组。
3. **硬件 PCB 设计**：将面包板电路沉淀到嘉立创 EDA 原理图与 PCB 掌机外壳中。
