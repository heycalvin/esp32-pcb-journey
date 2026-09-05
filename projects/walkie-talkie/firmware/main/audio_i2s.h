#pragma once

#include <esp_err.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化音频子系统 (INMP441 麦克风输入与 MAX98357A 扬声器输出)
 * @return esp_err_t ESP_OK 表示成功
 */
esp_err_t audio_i2s_init(void);

/**
 * @brief 从 INMP441 麦克风读取指定数量的 PCM 采样数据 (16-bit 单声道)
 * @param[out] buffer 输出音频数据缓冲区
 * @param[in] samples_to_read 期望读取的采样点数 (不是字节数)
 * @param[out] samples_read 实际读取到的采样点数
 * @param[in] timeout_ms 读取超时时间 (毫秒)
 * @return esp_err_t ESP_OK 表示成功
 */
esp_err_t audio_i2s_read_samples(int16_t *buffer, size_t samples_to_read, size_t *samples_read, uint32_t timeout_ms);

/**
 * @brief 向 MAX98357A 扬声器写入指定数量的 PCM 采样数据 (16-bit 单声道)
 * @param[in] buffer 输入音频数据缓冲区
 * @param[in] samples_to_write 期望写入的采样点数 (不是字节数)
 * @param[out] samples_written 实际写入的采样点数
 * @param[in] timeout_ms 写入超时时间 (毫秒)
 * @return esp_err_t ESP_OK 表示成功
 */
esp_err_t audio_i2s_write_samples(const int16_t *buffer, size_t samples_to_write, size_t *samples_written, uint32_t timeout_ms);

/**
 * @brief 静音扬声器 (向 DMA 写入一小段 0 数据并清空)
 */
void audio_i2s_mute_speaker(void);

#ifdef __cplusplus
}
#endif
