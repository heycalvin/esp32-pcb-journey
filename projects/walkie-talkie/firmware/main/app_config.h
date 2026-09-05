#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 运行模式配置
 * 0: 完整 ESP-NOW 双向对讲机模式 (Walkie-Talkie)
 * 1: 本地硬件自环测试模式 (Local Loopback: 麦克风直接出扬声器，排查接线)
 * ==================================================================== */
#define APP_MODE_WALKIE_TALKIE      0
#define APP_MODE_LOCAL_LOOPBACK     1

#define CURRENT_APP_MODE            APP_MODE_LOCAL_LOOPBACK

/* ====================================================================
 * 音频参数配置
 * ==================================================================== */
#define AUDIO_SAMPLE_RATE           8000        // 采样率: 8kHz (对讲通话标准)
#define AUDIO_BITS_PER_SAMPLE       16          // 采样位深: 16-bit
#define AUDIO_CHANNELS              1           // 单声道

// 单个音频分包采样数: 10ms 音频 = 8000 * 0.01 = 80 samples
// 80 samples * 2 bytes = 160 字节 (小于 ESP-NOW 250 字节单包上限)
#define AUDIO_SAMPLES_PER_PACKET    80
#define AUDIO_PACKET_PAYLOAD_SIZE   (AUDIO_SAMPLES_PER_PACKET * sizeof(int16_t))

// 抖动缓冲区容量配置 (单位: 字节)
// 160 字节 * 10 包 = 1600 字节 (约 100ms 缓冲延迟)
#define JITTER_BUFFER_SIZE          (AUDIO_PACKET_PAYLOAD_SIZE * 12)

/* ====================================================================
 * ESP-NOW 无线通信配置
 * ==================================================================== */
#define ESPNOW_WIFI_CHANNEL         1           // Wi-Fi 信道 (所有对讲机需保持一致)
#define ESPNOW_MAGIC_HEADER         0x57414C4B  // "WALK" 魔数标识

/* ====================================================================
 * 硬件引脚分配 (Pinout)
 * 默认适配标准 ESP32 (WROOM-32)，若是 ESP32-S3 可在此修改
 * ==================================================================== */
#if CONFIG_IDF_TARGET_ESP32S3
    // ESP32-S3 默认引脚
    #define I2S_MIC_SCK_PIN         41
    #define I2S_MIC_WS_PIN          42
    #define I2S_MIC_SD_PIN          2

    #define I2S_SPK_BCLK_PIN        16
    #define I2S_SPK_LRC_PIN         17
    #define I2S_SPK_DIN_PIN         15

    #define PTT_BUTTON_GPIO         4
    #define STATUS_LED_GPIO         48
#else
    // 标准 ESP32 (WROOM-32 / NodeMCU-32S) 默认引脚
    #define I2S_MIC_SCK_PIN         14          // INMP441 SCK (BCLK)
    #define I2S_MIC_WS_PIN          15          // INMP441 WS (LRCK)
    #define I2S_MIC_SD_PIN          32          // INMP441 SD (DOUT)

    #define I2S_SPK_BCLK_PIN        26          // MAX98357A BCLK
    #define I2S_SPK_LRC_PIN         25          // MAX98357A LRC
    #define I2S_SPK_DIN_PIN         22          // MAX98357A DIN

    #define PTT_BUTTON_GPIO         4           // PTT 按键 (内部上拉，按下为低电平0)
    #define STATUS_LED_GPIO         2           // 板载 LED 指示灯
#endif

#ifdef __cplusplus
}
#endif
