#pragma once

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_COLOR_STANDBY_BLUE,   // 待机: 蓝色
    LED_COLOR_TALK_GREEN,     // 按下说话: 绿色
    LED_COLOR_RELEASE_RED,    // 松开: 红色
    LED_COLOR_OFF             // 熄灭
} led_status_color_t;

esp_err_t status_led_init(void);
void status_led_set_status(led_status_color_t status);
void status_led_set_rgb(uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif
