/**
 * @file main.c
 * @author risinek (risinek@gmail.com)
 * @date 2021-04-03
 * @copyright Copyright (c) 2021
 * 
 * @brief Main file used to setup ESP32 into initial state
 * 
 * Starts management AP and webserver  
 */

#include <stdio.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_event.h"
#include "esp_system.h"

#include "attack.h"
#include "wifi_controller.h"
#include "webserver.h"
#include "version.h"

static const char* TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "%s v%s", APP_NAME, APP_VERSION);
    ESP_LOGI(TAG, "Build: %s %s", APP_BUILD_DATE, APP_BUILD_TIME);
    ESP_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());
    
    ESP_LOGD(TAG, "app_main started");
    esp_err_t ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return;
    }
    
    wifictl_mgmt_ap_start();
    attack_init();
    webserver_run();
    
    ESP_LOGI(TAG, "Application started successfully");
}
