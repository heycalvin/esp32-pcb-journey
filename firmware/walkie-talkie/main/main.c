#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "driver/gpio.h"

#include "app_config.h"
#include "audio_i2s.h"
#include "espnow_protocol.h"
#include "ptt_button.h"

static const char *TAG = "MAIN_APP";

#if CURRENT_APP_MODE == APP_MODE_LOCAL_LOOPBACK
/* ====================================================================
 * 阶段 1：单机硬件自环测试任务 (Local Mic -> Speaker Loopback)
 * 验证 INMP441 录音与 MAX98357A 放音硬件连接和音质
 * ==================================================================== */
static void local_loopback_task(void *pvParameters)
{
    ESP_LOGW(TAG, ">>> RUNNING IN LOCAL LOOPBACK TEST MODE <<<");
    ESP_LOGW(TAG, "Speak into the INMP441 microphone; voice will output directly to MAX98357A speaker.");

    int16_t audio_buffer[AUDIO_SAMPLES_PER_PACKET];
    size_t samples_read = 0;
    size_t samples_written = 0;

    while (1) {
        // 1. 从麦克风读取音频
        esp_err_t ret = audio_i2s_read_samples(audio_buffer, AUDIO_SAMPLES_PER_PACKET, &samples_read, 100);
        if (ret == ESP_OK && samples_read > 0) {
            // 2. 软件适度增益 (如果需要)
            for (size_t i = 0; i < samples_read; i++) {
                // 适度数字放大 2 倍
                int32_t val = (int32_t)audio_buffer[i] * 2;
                if (val > 32767) val = 32767;
                if (val < -32768) val = -32768;
                audio_buffer[i] = (int16_t)val;
            }

            // 3. 直接喂给扬声器
            audio_i2s_write_samples(audio_buffer, samples_read, &samples_written, 100);
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

#else
/* ====================================================================
 * 阶段 2~4：ESP-NOW 对讲机发送与接收任务
 * ==================================================================== */

// 音频采集与 ESP-NOW 发送任务
static void audio_tx_task(void *pvParameters)
{
    int16_t tx_buffer[AUDIO_SAMPLES_PER_PACKET];
    size_t samples_read = 0;
    bool was_talking = false;

    while (1) {
        if (ptt_button_is_pressed()) {
            if (!was_talking) {
                was_talking = true;
                ESP_LOGI(TAG, "[TX] PTT Pressed -> Starting Audio Broadcast...");
#if defined(STATUS_LED_GPIO) && (STATUS_LED_GPIO >= 0)
                gpio_set_level((gpio_num_t)STATUS_LED_GPIO, 1);
#endif
            }

            // 从 INMP441 录制 10ms 音频帧
            esp_err_t ret = audio_i2s_read_samples(tx_buffer, AUDIO_SAMPLES_PER_PACKET, &samples_read, 20);
            if (ret == ESP_OK && samples_read > 0) {
                // 通过 ESP-NOW 广播发送
                espnow_protocol_send_audio(tx_buffer, samples_read);
            }
        } else {
            if (was_talking) {
                was_talking = false;
                ESP_LOGI(TAG, "[TX] PTT Released -> Stopped Audio Broadcast.");
#if defined(STATUS_LED_GPIO) && (STATUS_LED_GPIO >= 0)
                gpio_set_level((gpio_num_t)STATUS_LED_GPIO, 0);
#endif
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

// ESP-NOW 接收与音频播放任务
static void audio_rx_task(void *pvParameters)
{
    int16_t rx_buffer[AUDIO_SAMPLES_PER_PACKET];
    size_t samples_received = 0;
    size_t samples_written = 0;

    while (1) {
        // 如果当前自己正在说话(按下PTT)，静音扬声器，防止喇叭啸叫与回声
        if (ptt_button_is_pressed()) {
            espnow_protocol_flush_rx_buffer();
            audio_i2s_mute_speaker();
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        // 从抖动缓冲区读取对端发来的音频
        bool ok = espnow_protocol_receive_audio(rx_buffer, AUDIO_SAMPLES_PER_PACKET, &samples_received, 20);
        if (ok && samples_received > 0) {
            audio_i2s_write_samples(rx_buffer, samples_received, &samples_written, 20);
        } else {
            // 无数据时休眠 5ms
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}
#endif

void app_main(void)
{
    ESP_LOGI(TAG, "=================================================");
    ESP_LOGI(TAG, "       ESP32 ESP-NOW Walkie-Talkie MVP           ");
    ESP_LOGI(TAG, "=================================================");

    // 1. 初始化 I2S 音频子系统 (麦克风 + 扬声器)
    ESP_ERROR_CHECK(audio_i2s_init());

#if CURRENT_APP_MODE == APP_MODE_LOCAL_LOOPBACK
    // 启动单机自环测试
    xTaskCreatePinnedToCore(local_loopback_task, "loopback_task", 4096, NULL, 5, NULL, 0);
#else
    // 2. 初始化 ESP-NOW 协议栈
    ESP_ERROR_CHECK(espnow_protocol_init());

    // 3. 初始化 PTT 按键
    ESP_ERROR_CHECK(ptt_button_init(NULL));

    // 4. 启动音频采集发送任务与接收播放任务
    xTaskCreatePinnedToCore(audio_tx_task, "audio_tx_task", 4096, NULL, 6, NULL, 0);
    xTaskCreatePinnedToCore(audio_rx_task, "audio_rx_task", 4096, NULL, 5, NULL, 1);
#endif

    ESP_LOGI(TAG, "System initialization complete. Ready!");
}
