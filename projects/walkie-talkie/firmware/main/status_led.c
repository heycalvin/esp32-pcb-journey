#include "status_led.h"
#include "app_config.h"
#include <esp_log.h>
#include "led_strip.h"

static const char *TAG = "STATUS_LED";
static led_strip_handle_t s_strip = NULL;

esp_err_t status_led_init(void)
{
    ESP_LOGI(TAG, "Initializing onboard WS2812 RGB LED on GPIO %d...", STATUS_LED_GPIO);

    led_strip_config_t strip_config = {
        .strip_gpio_num = STATUS_LED_GPIO,
        .max_leds = 1,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = {
            .invert_out = false,
        }
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags = {
            .with_dma = false,
        }
    };

    esp_err_t ret = led_strip_new_rmt_device(&strip_config, &rmt_config, &s_strip);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create led_strip RMT device: %s", esp_err_to_name(ret));
        return ret;
    }

    // 默认开机待机：显示蓝色
    status_led_set_status(LED_COLOR_STANDBY_BLUE);
    return ESP_OK;
}

void status_led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    if (s_strip) {
        led_strip_set_pixel(s_strip, 0, r, g, b);
        led_strip_refresh(s_strip);
    }
}

void status_led_set_status(led_status_color_t status)
{
    switch (status) {
        case LED_COLOR_TALK_GREEN:
            // 按下说话：亮绿色
            status_led_set_rgb(0, 50, 0);
            break;
        case LED_COLOR_RELEASE_RED:
            // 松开按键：亮红色
            status_led_set_rgb(50, 0, 0);
            break;
        case LED_COLOR_STANDBY_BLUE:
            // 待机监听：亮蓝色
            status_led_set_rgb(0, 0, 40);
            break;
        case LED_COLOR_OFF:
        default:
            status_led_set_rgb(0, 0, 0);
            break;
    }
}
