#pragma once

#include <stdbool.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ptt_state_change_cb_t)(bool is_pressed);

/**
 * @brief 初始化 PTT 按键 GPIO
 * @param[in] callback 按键状态改变时的回调函数 (可为 NULL)
 * @return esp_err_t ESP_OK 表示成功
 */
esp_err_t ptt_button_init(ptt_state_change_cb_t callback);

/**
 * @brief 查询当前 PTT 按键是否处于按下状态
 * @return true 按下 (Talking), false 松开 (Listening)
 */
bool ptt_button_is_pressed(void);

#ifdef __cplusplus
}
#endif
