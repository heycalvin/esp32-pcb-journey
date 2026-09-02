#include "audio_i2s.h"
#include "app_config.h"
#include <esp_log.h>
#include <string.h>

#include "driver/i2s_std.h"
#include "driver/gpio.h"

static const char *TAG = "AUDIO_I2S";

static i2s_chan_handle_t s_tx_chan = NULL; // 扬声器 (MAX98357A)
static i2s_chan_handle_t s_rx_chan = NULL; // 麦克风 (INMP441)

esp_err_t audio_i2s_init(void)
{
    ESP_LOGI(TAG, "Initializing I2S audio subsystem (Sample Rate: %d Hz)...", AUDIO_SAMPLE_RATE);

    /* -------------------------------------------------------------
     * 1. 初始化 TX 通道 (MAX98357A 扬声器驱动)
     * ------------------------------------------------------------- */
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    tx_chan_cfg.dma_desc_num = 6;
    tx_chan_cfg.dma_frame_num = AUDIO_SAMPLES_PER_PACKET;
    tx_chan_cfg.auto_clear = true;

    esp_err_t ret = i2s_new_channel(&tx_chan_cfg, &s_tx_chan, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to allocate I2S TX channel: %s", esp_err_to_name(ret));
        return ret;
    }

    i2s_std_config_t tx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(AUDIO_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = (gpio_num_t)I2S_SPK_BCLK_PIN,
            .ws   = (gpio_num_t)I2S_SPK_LRC_PIN,
            .dout = (gpio_num_t)I2S_SPK_DIN_PIN,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    tx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;

    ret = i2s_channel_init_std_mode(s_tx_chan, &tx_std_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure I2S TX channel: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = i2s_channel_enable(s_tx_chan);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable I2S TX channel: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Speaker I2S TX channel enabled successfully (BCLK:%d, LRC:%d, DIN:%d)",
             I2S_SPK_BCLK_PIN, I2S_SPK_LRC_PIN, I2S_SPK_DIN_PIN);

    /* -------------------------------------------------------------
     * 2. 初始化 RX 通道 (INMP441 麦克风采集)
     * ------------------------------------------------------------- */
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    rx_chan_cfg.dma_desc_num = 6;
    rx_chan_cfg.dma_frame_num = AUDIO_SAMPLES_PER_PACKET;

    ret = i2s_new_channel(&rx_chan_cfg, NULL, &s_rx_chan);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to allocate I2S RX channel: %s", esp_err_to_name(ret));
        return ret;
    }

    i2s_std_config_t rx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(AUDIO_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = (gpio_num_t)I2S_MIC_SCK_PIN,
            .ws   = (gpio_num_t)I2S_MIC_WS_PIN,
            .dout = I2S_GPIO_UNUSED,
            .din  = (gpio_num_t)I2S_MIC_SD_PIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;

    ret = i2s_channel_init_std_mode(s_rx_chan, &rx_std_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure I2S RX channel: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = i2s_channel_enable(s_rx_chan);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable I2S RX channel: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Microphone I2S RX channel enabled successfully (SCK:%d, WS:%d, SD:%d)",
             I2S_MIC_SCK_PIN, I2S_MIC_WS_PIN, I2S_MIC_SD_PIN);

    return ESP_OK;
}

esp_err_t audio_i2s_read_samples(int16_t *buffer, size_t samples_to_read, size_t *samples_read, uint32_t timeout_ms)
{
    if (!s_rx_chan || !buffer) {
        return ESP_ERR_INVALID_STATE;
    }

    size_t bytes_to_read = samples_to_read * sizeof(int16_t);
    size_t bytes_read = 0;

    esp_err_t ret = i2s_channel_read(s_rx_chan, buffer, bytes_to_read, &bytes_read, pdMS_TO_TICKS(timeout_ms));
    if (samples_read) {
        *samples_read = bytes_read / sizeof(int16_t);
    }
    return ret;
}

esp_err_t audio_i2s_write_samples(const int16_t *buffer, size_t samples_to_write, size_t *samples_written, uint32_t timeout_ms)
{
    if (!s_tx_chan || !buffer) {
        return ESP_ERR_INVALID_STATE;
    }

    size_t bytes_to_write = samples_to_write * sizeof(int16_t);
    size_t bytes_written = 0;

    esp_err_t ret = i2s_channel_write(s_tx_chan, buffer, bytes_to_write, &bytes_written, pdMS_TO_TICKS(timeout_ms));
    if (samples_written) {
        *samples_written = bytes_written / sizeof(int16_t);
    }
    return ret;
}

void audio_i2s_mute_speaker(void)
{
    if (!s_tx_chan) return;
    int16_t silence[AUDIO_SAMPLES_PER_PACKET] = {0};
    size_t written = 0;
    i2s_channel_write(s_tx_chan, silence, sizeof(silence), &written, 10);
}
