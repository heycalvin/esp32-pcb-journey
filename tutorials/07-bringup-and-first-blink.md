# 07. 验证篇：硬件 Bring-up 与点灯测试

当你收到刚出炉的 PCB 板子后，**千万不要一上来就插电脑 USB！**  
请按照标准的硬件上电流程（Bring-up）进行检测，防止短路烧坏电脑 USB 接口或芯片。

---

## 1. 第一步：冒烟测试前——万用表测短路 (Cold Check)

1. 将万用表打到 **蜂鸣档 (通断档)** 或 **电阻档**。
2. 测量 `5V (VBUS)` 与 `GND` 之间：
   - 蜂鸣器**绝对不能响**！电阻应该显示为无穷大或几百 kΩ。
3. 测量 `3.3V` 与 `GND` 之间：
   - 蜂鸣器**绝对不能响**！
4. 如果响了或电阻接近 0Ω，说明焊锡桥接短路，必须用吸锡带或烙铁排查排除后再继续！

---

## 2. 第二步：空载上电量电压 (Power Check)

1. 插入 Type-C 线供电（可以先插移动电源或带过流保护的充电头）。
2. 将万用表打到 **直流电压档 (DC 20V)**。
3. 红表笔测 5V 引脚，黑表笔测 GND：应显示约 **4.9V ~ 5.2V**。
4. 红表笔测 3.3V 引脚，黑表笔测 GND：应精准显示 **3.28V ~ 3.33V**。
5. 手摸 LDO 稳压芯片和 ESP32 芯片表面：应处于常温或微温，若**瞬间发烫烫手**立即拔掉断电排查！

---

## 3. 第三步：插电脑测串口与识别 (Port Check)

1. 将板子插上电脑 USB 接口。
2. 打开设备管理器（Windows）：
   - 展开 **端口 (COM 和 LPT)**。
   - 应该能看到诸如 `USB-Enhanced-SERIAL CH343 (COMx)` 或 `USB-SERIAL CH340 (COMx)`。
3. 如果看到黄色感叹号，去沁恒官网下载安装对应 CH340/CH343 驱动即可。

---

## 4. 第四步：编写第一个点灯固件 (Blink)

在 Arduino IDE 或 ESP-IDF 中编写一段极简的 LED 闪烁测试代码：

```cpp
#include <Arduino.h>

// 假设你在 PCB 上把板载 LED 接到了 GPIO2 (或者 GPIO8)
#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("Hello ESP32 PCB Journey! System Initialized.");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(500);

  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(500);
}
```

点击 **上传 (Upload)**，观察控制台是否自动连接、擦除 Flash、烧录完成并看到板载 LED 规律闪烁。  
🎉 **恭喜你！你已经成功走通了属于你的第一块 ESP32 硬件全生命周期！**
