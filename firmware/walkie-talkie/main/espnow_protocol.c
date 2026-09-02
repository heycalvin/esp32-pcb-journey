#include "espnow_protocol.h"
#include <esp_wifi.h>
#include <esp_now.h>
#include <esp_log.h>
#include <esp_idf_version.h>
#include <nvs_flash.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/ringbuf.h>

static const char *TAG = "ESPNOW_PROTO";

static const uint8_t s_broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t s_local_mac[6] = {0};
static uint16_t s_tx_seq = 0;
static RingbufHandle_t s_rx_ring_buffer = NULL;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    const uint8_t *src_mac = recv_info->src_addr;
#else
static void espnow_recv_cb(const uint8_t *src_mac, const uint8_t *data, int len)
{
#endif
    if (len < (int)sizeof(espnow_audio_packet_t)) {
        return;
    }

    const espnow_audio_packet_t *pkt = (const espnow_audio_packet_t *)data;

    // 校验魔数与自发自收过滤
    if (pkt->magic != ESPNOW_MAGIC_HEADER) {
        return;
    }
    if (memcmp(src_mac, s_local_mac, 6) == 0) {
        return; // 过滤本机自己广播出去的包
    }

    if (pkt->payload_len > 0 && s_rx_ring_buffer != NULL) {
        // 将接收到的 PCM 数据推入环形缓冲区
        BaseType_t ret = xRingbufferSend(s_rx_ring_buffer, pkt->data, pkt->payload_len, 0);
        if (ret != pdTRUE) {
            // 缓冲区满，丢弃旧数据或跳过以保证实时性 (防止累积高延迟)
            ESP_LOGD(TAG, "RX Jitter Buffer full, dropping packet seq:%d", pkt->seq_num);
        }
    }
}

static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    // 广播发送通常 status 为 0，此处可用于调试统计
}

esp_err_t espnow_protocol_init(void)
{
    ESP_LOGI(TAG, "Initializing Wi-Fi Station & ESP-NOW Protocol...");

    // 1. 初始化 NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. 初始化网络与 Wi-Fi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // 固定 Wi-Fi 信道
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));

    // 获取本机 MAC 地址
    esp_wifi_get_mac(WIFI_IF_STA, s_local_mac);
    ESP_LOGI(TAG, "Local Device MAC: %02X:%02X:%02X:%02X:%02X:%02X, Channel: %d",
             s_local_mac[0], s_local_mac[1], s_local_mac[2],
             s_local_mac[3], s_local_mac[4], s_local_mac[5], ESPNOW_WIFI_CHANNEL);

    // 3. 创建接收环形缓冲区
    s_rx_ring_buffer = xRingbufferCreate(JITTER_BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    if (!s_rx_ring_buffer) {
        ESP_LOGE(TAG, "Failed to create Jitter Ring Buffer!");
        return ESP_ERR_NO_MEM;
    }

    // 4. 初始化 ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

    // 5. 添加全局广播 Peer (免配对即开即通)
    esp_now_peer_info_t peer_info = {0};
    memcpy(peer_info.peer_addr, s_broadcast_mac, ESP_NOW_ETH_ALEN);
    peer_info.channel = ESPNOW_WIFI_CHANNEL;
    peer_info.ifidx = WIFI_IF_STA;
    peer_info.encrypt = false;

    ret = esp_now_add_peer(&peer_info);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add ESP-NOW broadcast peer: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "ESP-NOW Walkie-Talkie Protocol Initialized Successfully.");
    return ESP_OK;
}

esp_err_t espnow_protocol_send_audio(const int16_t *pcm_samples, size_t sample_count)
{
    if (!pcm_samples || sample_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    espnow_audio_packet_t pkt;
    pkt.magic = ESPNOW_MAGIC_HEADER;
    pkt.seq_num = s_tx_seq++;
    pkt.node_id = s_local_mac[5];

    size_t bytes_to_copy = sample_count * sizeof(int16_t);
    if (bytes_to_copy > AUDIO_PACKET_PAYLOAD_SIZE) {
        bytes_to_copy = AUDIO_PACKET_PAYLOAD_SIZE;
    }
    pkt.payload_len = (uint8_t)bytes_to_copy;
    memcpy(pkt.data, pcm_samples, bytes_to_copy);

    return esp_now_send(s_broadcast_mac, (const uint8_t *)&pkt, sizeof(pkt));
}

bool espnow_protocol_receive_audio(int16_t *buffer, size_t max_samples, size_t *samples_out, uint32_t timeout_ms)
{
    if (!s_rx_ring_buffer || !buffer) {
        return false;
    }

    size_t bytes_needed = max_samples * sizeof(int16_t);
    size_t item_size = 0;

    // 从 RingBuffer 中获取字节流
    uint8_t *item = (uint8_t *)xRingbufferReceiveUpTo(
        s_rx_ring_buffer,
        &item_size,
        pdMS_TO_TICKS(timeout_ms),
        bytes_needed
    );

    if (item != NULL && item_size > 0) {
        memcpy(buffer, item, item_size);
        vRingbufferReturnItem(s_rx_ring_buffer, (void *)item);
        if (samples_out) {
            *samples_out = item_size / sizeof(int16_t);
        }
        return true;
    }

    if (samples_out) {
        *samples_out = 0;
    }
    return false;
}

void espnow_protocol_flush_rx_buffer(void)
{
    if (!s_rx_ring_buffer) return;

    size_t item_size = 0;
    while (1) {
        uint8_t *item = (uint8_t *)xRingbufferReceive(s_rx_ring_buffer, &item_size, 0);
        if (!item) break;
        vRingbufferReturnItem(s_rx_ring_buffer, (void *)item);
    }
}
