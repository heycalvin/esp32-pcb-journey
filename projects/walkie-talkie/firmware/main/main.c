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
#include "status_led.h"

#include <freertos/timers.h>

static const char *TAG = "MAIN_APP";
static TimerHandle_t s_release_timer = NULL;

static void release_timer_cb(TimerHandle_t xTimer)
{
    status_led_set_status(LED_COLOR_STANDBY_BLUE);
}

static void on_ptt_state_changed(bool is_pressed)
{
    if (is_pressed) {
        if (s_release_timer) {
            xTimerStop(s_release_timer, 0);
        }
        status_led_set_status(LED_COLOR_TALK_GREEN); // 按下：绿色
    } else {
        status_led_set_status(LED_COLOR_RELEASE_RED); // 松开：红色
        if (s_release_timer) {
            xTimerStart(s_release_timer, 0); // 2秒后回待机蓝
        }
    }
}

static void play_startup_beep(void)
{
    ESP_LOGI(TAG, "Playing startup test tone on speaker (1000Hz)...");
    int16_t wave[8] = {0, 12000, 20000, 12000, 0, -12000, -20000, -12000};
    int16_t buf[AUDIO_SAMPLES_PER_PACKET];
    size_t written = 0;
    for (int rep = 0; rep < 40; rep++) { // 400ms
        for (int i = 0; i < AUDIO_SAMPLES_PER_PACKET; i++) {
            buf[i] = wave[i % 8];
        }
        audio_i2s_write_samples(buf, AUDIO_SAMPLES_PER_PACKET, &written, 50);
    }
    ESP_LOGI(TAG, "Startup test tone finished.");
}

#if CURRENT_APP_MODE == APP_MODE_LOCAL_LOOPBACK
/* ====================================================================
 * 阶段 1：单机硬件自环测试任务 (Local Mic -> Speaker Loopback)
 * 验证 INMP441 录音与 MAX98357A 放音硬件连接和音质
 * ==================================================================== */
#define VOICE_MEMO_MAX_SAMPLES (AUDIO_SAMPLE_RATE * 4) // 最长录音 4 秒 (32,000 samples)
static int16_t s_voice_memo[VOICE_MEMO_MAX_SAMPLES];

static void local_loopback_task(void *pvParameters)
{
    ESP_LOGW(TAG, ">>> RUNNING IN VOICE MEMO TEST MODE <<<");
    ESP_LOGW(TAG, "Hold button to record voice (up to 4s), release button to hear crystal clear playback!");

    int16_t audio_buffer[AUDIO_SAMPLES_PER_PACKET];
    size_t samples_read = 0;
    size_t recorded_samples = 0;
    bool was_pressed = false;

    while (1) {
        bool is_pressed = ptt_button_is_pressed();

        if (is_pressed) {
            if (!was_pressed) {
                was_pressed = true;
                recorded_samples = 0;
                audio_i2s_mute_speaker();
                ESP_LOGI(TAG, "=== [RECORDING] Please speak into microphone... ===");
            }

            // 读取音频并存入录音缓冲区
            esp_err_t ret = audio_i2s_read_samples(audio_buffer, AUDIO_SAMPLES_PER_PACKET, &samples_read, 20);
            if (ret == ESP_OK && samples_read > 0) {
                for (size_t i = 0; i < samples_read; i++) {
                    if (recorded_samples < VOICE_MEMO_MAX_SAMPLES) {
                        int32_t amplified = (int32_t)audio_buffer[i] * 5;
                        if (amplified > 32767) amplified = 32767;
                        if (amplified < -32768) amplified = -32768;
                        s_voice_memo[recorded_samples++] = (int16_t)amplified;
                    }
                }
            }
        } else {
            if (was_pressed) {
                was_pressed = false;
                ESP_LOGI(TAG, "=== [PLAYBACK] Playing back %d samples through speaker! ===", (int)recorded_samples);

                // 松开按键：立即从喇叭清晰回放刚才录制的声音！
                size_t written = 0;
                size_t offset = 0;
                while (offset < recorded_samples) {
                    size_t chunk = recorded_samples - offset;
                    if (chunk > AUDIO_SAMPLES_PER_PACKET) chunk = AUDIO_SAMPLES_PER_PACKET;
                    audio_i2s_write_samples(&s_voice_memo[offset], chunk, &written, 50);
                    offset += chunk;
                }
                audio_i2s_mute_speaker();
                ESP_LOGI(TAG, "=== [PLAYBACK COMPLETE] ===");
            }
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
            }

            // 从 INMP441 录制 10ms 音频帧
            esp_err_t ret = audio_i2s_read_samples(tx_buffer, AUDIO_SAMPLES_PER_PACKET, &samples_read, 20);
            if (ret == ESP_OK && samples_read > 0) {
                // 适度数字放大 6 倍，确保对端听到清晰响亮的人声
                for (size_t i = 0; i < samples_read; i++) {
                    int32_t amp = (int32_t)tx_buffer[i] * 6;
                    if (amp > 32767) amp = 32767;
                    if (amp < -32768) amp = -32768;
                    tx_buffer[i] = (int16_t)amp;
                }
                // 通过 ESP-NOW 广播发送给对端
                espnow_protocol_send_audio(tx_buffer, samples_read);

                // 本地实时耳返回显：直接喂给本地喇叭，单机也能亲耳确认麦克风和喇叭完全正常
                size_t written = 0;
                audio_i2s_write_samples(tx_buffer, samples_read, &written, 20);
            }
        } else {
            if (was_talking) {
                was_talking = false;
                ESP_LOGI(TAG, "[TX] PTT Released -> Stopped Audio Broadcast.");
                audio_i2s_mute_speaker();
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

    // 1. 初始化板载 WS2812 状态指示灯 (待机: 蓝色)
    status_led_init();

    // 创建按键松开红灯延时定时器 (2秒后恢复待机蓝)
    s_release_timer = xTimerCreate("rel_tmr", pdMS_TO_TICKS(2000), pdFALSE, NULL, release_timer_cb);

    // 2. 初始化 I2S 音频子系统 (麦克风 + 扬声器)
    ESP_ERROR_CHECK(audio_i2s_init());

    // 3. 初始化 PTT 按键 (挂载按键状态改变回调)
    ESP_ERROR_CHECK(ptt_button_init(on_ptt_state_changed));

    // 4. 扬声器硬件自检：开机播放 400ms 提示音
    play_startup_beep();

#if CURRENT_APP_MODE == APP_MODE_LOCAL_LOOPBACK
    // 启动单机自环测试
    xTaskCreatePinnedToCore(local_loopback_task, "loopback_task", 4096, NULL, 5, NULL, 0);
#else
    // 4. 初始化 ESP-NOW 协议栈
    ESP_ERROR_CHECK(espnow_protocol_init());

    // 5. 启动音频采集发送任务与接收播放任务
    xTaskCreatePinnedToCore(audio_tx_task, "audio_tx_task", 4096, NULL, 6, NULL, 0);
    xTaskCreatePinnedToCore(audio_rx_task, "audio_rx_task", 4096, NULL, 5, NULL, 1);
#endif

    ESP_LOGI(TAG, "System initialization complete. Ready!");
}
