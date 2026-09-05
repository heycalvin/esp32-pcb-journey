#pragma once

#include <esp_err.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "app_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP-NOW 音频数据包结构体 (字节对齐)
 */
typedef struct __attribute__((packed)) {
    uint32_t magic;                                 // 魔数标识 (0x57414C4B "WALK")
    uint16_t seq_num;                               // 递增包序列号
    uint8_t  node_id;                               // 发送者简短标识 (例如 MAC 后一字节)
    uint8_t  payload_len;                           // 有效音频数据长度 (字节数)
    uint8_t  data[AUDIO_PACKET_PAYLOAD_SIZE];       // 原始 16-bit PCM 音频采样数据
} espnow_audio_packet_t;

/**
 * @brief 初始化 Wi-Fi 与 ESP-NOW 通信协议栈
 * @return esp_err_t ESP_OK 表示成功
 */
esp_err_t espnow_protocol_init(void);

/**
 * @brief 广播发送一段音频数据包
 * @param[in] pcm_samples 16-bit PCM 采样数组
 * @param[in] sample_count 采样点数量 (应为 AUDIO_SAMPLES_PER_PACKET)
 * @return esp_err_t ESP_OK 表示发送成功
 */
esp_err_t espnow_protocol_send_audio(const int16_t *pcm_samples, size_t sample_count);

/**
 * @brief 从接收抖动缓冲区获取待播放的音频数据
 * @param[out] buffer 输出 PCM 目标缓冲区
 * @param[in] max_samples 期望读取的最大采样数
 * @param[out] samples_out 实际读取的采样数
 * @param[in] timeout_ms 等待超时 (毫秒)
 * @return bool true 获取成功, false 缓冲区无足够数据
 */
bool espnow_protocol_receive_audio(int16_t *buffer, size_t max_samples, size_t *samples_out, uint32_t timeout_ms);

/**
 * @brief 清空接收抖动缓冲区
 */
void espnow_protocol_flush_rx_buffer(void);

#ifdef __cplusplus
}
#endif
