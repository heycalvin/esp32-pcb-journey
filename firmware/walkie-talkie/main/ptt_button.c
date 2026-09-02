#include "ptt_button.h"
#include "app_config.h"
#include <esp_log.h>
#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *TAG = "PTT_BTN";

static ptt_state_change_cb_t s_state_cb = NULL;
static bool s_last_state = false;

static void ptt_poll_task(void *pvParameters)
{
    while (1) {
        // 低电平有效 (按下为0)
        bool pressed = (gpio_get_level((gpio_num_t)PTT_BUTTON_GPIO) == 0);

        if (pressed != s_last_state) {
            vTaskDelay(pdMS_TO_TICKS(20)); // 软件防抖
            pressed = (gpio_get_level((gpio_num_t)PTT_BUTTON_GPIO) == 0);
            if (pressed != s_last_state) {
                s_last_state = pressed;
                ESP_LOGI(TAG, "PTT Button State Changed: %s", pressed ? "PRESSED (TALK)" : "RELEASED (LISTEN)");
                if (s_state_cb) {
                    s_state_cb(pressed);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

esp_err_t ptt_button_init(ptt_state_change_cb_t callback)
{
    s_state_cb = callback;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PTT_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure PTT GPIO %d: %s", PTT_BUTTON_GPIO, esp_err_to_name(ret));
        return ret;
    }

    // LED 指示灯配置 (可选)
#if defined(STATUS_LED_GPIO) && (STATUS_LED_GPIO >= 0)
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << STATUS_LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&led_conf);
    gpio_set_level((gpio_num_t)STATUS_LED_GPIO, 0);
#endif

    xTaskCreatePinnedToCore(ptt_poll_task, "ptt_poll_task", 2048, NULL, 5, NULL, 1);
    ESP_LOGI(TAG, "PTT button initialized on GPIO %d (Active LOW)", PTT_BUTTON_GPIO);
    return ESP_OK;
}

bool ptt_button_is_pressed(void)
{
    return (gpio_get_level((gpio_num_t)PTT_BUTTON_GPIO) == 0);
}
