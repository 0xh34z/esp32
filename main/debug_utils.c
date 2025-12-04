/**
 * @file debug_utils.c
 * @author risinek (risinek@gmail.com)
 * @date 2025-12-03
 * @copyright Copyright (c) 2025
 * 
 * @brief Implementation of debug utilities
 */

#include "debug_utils.h"
#include <stdio.h>
#include "esp_heap_caps.h"
#include "esp_system.h"

static size_t last_free_size = 0;

void debug_print_memory_stats(void)
{
    size_t free_size = esp_get_free_heap_size();
    size_t min_free = esp_get_minimum_free_heap_size();
    size_t largest_block = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);
    
    ESP_LOGI("DEBUG", "=== Memory Statistics ===");
    ESP_LOGI("DEBUG", "Free heap: %u bytes", free_size);
    ESP_LOGI("DEBUG", "Minimum free: %u bytes", min_free);
    ESP_LOGI("DEBUG", "Largest block: %u bytes", largest_block);
    ESP_LOGI("DEBUG", "========================");
}

void debug_print_mac(const char *tag, const char *label, const uint8_t *mac)
{
    if (mac == NULL) {
        ESP_LOGE(tag, "%s: NULL MAC address", label);
        return;
    }
    
    ESP_LOGI(tag, "%s: %02X:%02X:%02X:%02X:%02X:%02X",
             label, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void debug_hexdump(const char *tag, const char *label, const uint8_t *data, size_t len)
{
    if (data == NULL) {
        ESP_LOGE(tag, "%s: NULL data pointer", label);
        return;
    }
    
    if (len == 0) {
        ESP_LOGW(tag, "%s: zero length", label);
        return;
    }
    
    ESP_LOGI(tag, "%s (%u bytes):", label, len);
    
    char hex_str[49]; // 16 bytes * 3 chars + null
    char ascii_str[17]; // 16 chars + null
    
    for (size_t i = 0; i < len; i += 16) {
        size_t line_len = (len - i < 16) ? (len - i) : 16;
        
        // Build hex representation
        for (size_t j = 0; j < 16; j++) {
            if (j < line_len) {
                sprintf(&hex_str[j * 3], "%02X ", data[i + j]);
                ascii_str[j] = (data[i + j] >= 32 && data[i + j] <= 126) ? data[i + j] : '.';
            } else {
                sprintf(&hex_str[j * 3], "   ");
                ascii_str[j] = ' ';
            }
        }
        ascii_str[16] = '\0';
        
        ESP_LOGI(tag, "%04X: %s %s", i, hex_str, ascii_str);
    }
}

void debug_check_memory_leak(const char *tag, const char *checkpoint_name)
{
    size_t current_free = esp_get_free_heap_size();
    
    if (last_free_size == 0) {
        last_free_size = current_free;
        ESP_LOGI(tag, "[%s] Memory checkpoint set: %u bytes free", 
                 checkpoint_name, current_free);
        return;
    }
    
    int32_t diff = (int32_t)(current_free - last_free_size);
    
    if (diff < 0) {
        ESP_LOGW(tag, "[%s] Possible memory leak: %d bytes lost", 
                 checkpoint_name, -diff);
    } else if (diff > 0) {
        ESP_LOGI(tag, "[%s] Memory freed: %d bytes", 
                 checkpoint_name, diff);
    } else {
        ESP_LOGI(tag, "[%s] No memory change detected", checkpoint_name);
    }
    
    last_free_size = current_free;
}
