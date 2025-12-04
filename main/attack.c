/**
 * @file attack.c
 * @author risinek (risinek@gmail.com)
 * @date 2021-04-02
 * @copyright Copyright (c) 2021
 * 
 * @brief Implements common attack wrapper.
 */

#include "attack.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "attack_pmkid.h"
#include "attack_handshake.h"
#include "attack_dos.h"
#include "webserver.h"
#include "wifi_controller.h"

#define NVS_NAMESPACE "attack_data"
#define NVS_KEY_STATE "state"
#define NVS_KEY_TYPE "type"
#define NVS_KEY_CONTENT_SIZE "content_size"
#define NVS_KEY_CONTENT "content"

static const char* TAG = "attack";
static attack_status_t attack_status = { .state = READY, .type = -1, .content_size = 0, .content = NULL };
static esp_timer_handle_t attack_timeout_handle;

const attack_status_t *attack_get_status() {
    return &attack_status;
}

void attack_update_status(attack_state_t state) {
    attack_status.state = state;
    if(state == FINISHED) {
        ESP_LOGD(TAG, "Stopping attack timeout timer");
        ESP_ERROR_CHECK(esp_timer_stop(attack_timeout_handle));
        
        // Save results to flash when finished
        if (attack_status.content_size > 0) {
            ESP_LOGI(TAG, "Saving finished results to flash...");
            attack_save_results_to_flash();
        }
    } 
}

void attack_append_status_content(uint8_t *buffer, unsigned size){
    if(size == 0){
        ESP_LOGE(TAG, "Size can't be 0 if you want to reallocate");
        return;
    }
    // temporarily save new location in case of realloc failure to preserve current content
    char *reallocated_content = realloc(attack_status.content, attack_status.content_size + size);
    if(reallocated_content == NULL){
        ESP_LOGE(TAG, "Error reallocating status content! Status content may not be complete.");
        return;
    }
    // copy new data after current content
    memcpy(&reallocated_content[attack_status.content_size], buffer, size);
    attack_status.content = reallocated_content;
    attack_status.content_size += size;
}

char *attack_alloc_result_content(unsigned size) {
    if (size == 0) {
        ESP_LOGW(TAG, "Attempting to allocate 0 bytes");
        return NULL;
    }
    
    // Free existing content if present
    if (attack_status.content != NULL) {
        ESP_LOGD(TAG, "Freeing existing content before new allocation");
        free(attack_status.content);
        attack_status.content = NULL;
        attack_status.content_size = 0;
    }
    
    attack_status.content_size = size;
    attack_status.content = (char *) malloc(size);
    if (attack_status.content == NULL) {
        ESP_LOGE(TAG, "Failed to allocate %u bytes for attack content", size);
        attack_status.content_size = 0;
    }
    return attack_status.content;
}

esp_err_t attack_save_results_to_flash() {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS handle: %s", esp_err_to_name(err));
        return err;
    }

    // Save state, type, and content size
    err = nvs_set_u8(nvs_handle, NVS_KEY_STATE, attack_status.state);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save state to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_set_u8(nvs_handle, NVS_KEY_TYPE, attack_status.type);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save type to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_set_u16(nvs_handle, NVS_KEY_CONTENT_SIZE, attack_status.content_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save content size to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    // Save content if present
    if (attack_status.content != NULL && attack_status.content_size > 0) {
        err = nvs_set_blob(nvs_handle, NVS_KEY_CONTENT, attack_status.content, attack_status.content_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to save content to NVS: %s", esp_err_to_name(err));
            nvs_close(nvs_handle);
            return err;
        }
        ESP_LOGI(TAG, "Saved %d bytes of attack results to flash", attack_status.content_size);
    }

    // Commit changes
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit to NVS: %s", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
    return err;
}

esp_err_t attack_load_results_from_flash() {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGD(TAG, "No saved results found in flash");
        } else {
            ESP_LOGE(TAG, "Failed to open NVS handle: %s", esp_err_to_name(err));
        }
        return err;
    }

    // Load state, type, and content size
    uint8_t state, type;
    uint16_t content_size;

    err = nvs_get_u8(nvs_handle, NVS_KEY_STATE, &state);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_get_u8(nvs_handle, NVS_KEY_TYPE, &type);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_get_u16(nvs_handle, NVS_KEY_CONTENT_SIZE, &content_size);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return err;
    }

    // Load content if present
    if (content_size > 0) {
        // Free existing content first
        if (attack_status.content != NULL) {
            free(attack_status.content);
        }

        attack_status.content = (char *)malloc(content_size);
        if (attack_status.content == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for loaded content");
            nvs_close(nvs_handle);
            return ESP_ERR_NO_MEM;
        }

        size_t required_size = content_size;
        err = nvs_get_blob(nvs_handle, NVS_KEY_CONTENT, attack_status.content, &required_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to load content from NVS: %s", esp_err_to_name(err));
            free(attack_status.content);
            attack_status.content = NULL;
            nvs_close(nvs_handle);
            return err;
        }

        attack_status.state = state;
        attack_status.type = type;
        attack_status.content_size = content_size;
        ESP_LOGI(TAG, "Loaded %d bytes of attack results from flash", content_size);
    }

    nvs_close(nvs_handle);
    return ESP_OK;
}

/**
 * @brief Callback function for attack timeout timer.
 * 
 * This function is called when attack times out. 
 * It updates attack status state to TIMEOUT.
 * It calls appropriate abort functions based on current attack type.
 * @param arg not used.
 */
static void attack_timeout(void* arg){
    ESP_LOGD(TAG, "Attack timed out");
    
    attack_update_status(TIMEOUT);

    switch(attack_status.type) {
        case ATTACK_TYPE_PMKID:
            ESP_LOGI(TAG, "Aborting PMKID attack...");
            attack_pmkid_stop();
            break;
        case ATTACK_TYPE_HANDSHAKE:
            ESP_LOGI(TAG, "Abort HANDSHAKE attack...");
            attack_handshake_stop();
            break;
        case ATTACK_TYPE_PASSIVE:
            ESP_LOGI(TAG, "Abort PASSIVE attack...");
            break;
        case ATTACK_TYPE_DOS:
            ESP_LOGI(TAG, "Abort DOS attack...");
            attack_dos_stop();
            break;
        default:
            ESP_LOGE(TAG, "Unknown attack type. Not aborting anything");
    }
    
    // Save results to flash after timeout
    if (attack_status.content_size > 0) {
        ESP_LOGI(TAG, "Saving timeout results to flash...");
        attack_save_results_to_flash();
    }
}

/**
 * @brief Callback for WEBSERVER_EVENT_ATTACK_REQUEST event.
 * 
 * This function handles WEBSERVER_EVENT_ATTACK_REQUEST event from event loop.
 * It parses attack_request_t structure and set initial values to attack_status.
 * It sets attack state to RUNNING.
 * It starts attack timeout timer.
 * It starts attack based on chosen type.
 * 
 * @param args not used
 * @param event_base expects WEBSERVER_EVENTS
 * @param event_id expects WEBSERVER_EVENT_ATTACK_REQUEST
 * @param event_data expects attack_request_t
 */
static void attack_request_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI(TAG, "Starting attack...");
    attack_request_t *attack_request = (attack_request_t *) event_data;
    attack_config_t attack_config = { .type = attack_request->type, .method = attack_request->method, .timeout = attack_request->timeout };
    attack_config.ap_record = wifictl_get_ap_record(attack_request->ap_record_id);
    
    attack_status.state = RUNNING;
    attack_status.type = attack_config.type;

    if(attack_config.ap_record == NULL){
        ESP_LOGE(TAG, "NPE: No attack_config.ap_record!");
        return;
    }
    // set timeout
    ESP_ERROR_CHECK(esp_timer_start_once(attack_timeout_handle, attack_config.timeout * 1000000));
    // start attack based on it's type
    switch(attack_config.type) {
        case ATTACK_TYPE_PMKID:
            attack_pmkid_start(&attack_config);
            break;
        case ATTACK_TYPE_HANDSHAKE:
            attack_handshake_start(&attack_config);
            break;
        case ATTACK_TYPE_PASSIVE:
            ESP_LOGW(TAG, "ATTACK_TYPE_PASSIVE not implemented yet!");
            break;
        case ATTACK_TYPE_DOS:
            attack_dos_start(&attack_config);
            break;
        default:
            ESP_LOGE(TAG, "Unknown attack type!");
    }
}

/**
 * @brief Callback for WEBSERVER_EVENT_ATTACK_RESET event.
 * 
 * This callback resets attack status by freeing previously allocated status content and putting attack to READY state.
 * 
 * @param args not used
 * @param event_base expects WEBSERVER_EVENTS
 * @param event_id expects WEBSERVER_EVENT_ATTACK_RESET
 * @param event_data not used
 */
static void attack_reset_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Resetting attack status...");
    if(attack_status.content){
        free(attack_status.content);
        attack_status.content = NULL;
    }
    attack_status.content_size = 0;
    attack_status.type = -1;
    attack_status.state = READY;
    
    // Clear saved results from flash
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_erase_all(nvs_handle);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        ESP_LOGI(TAG, "Cleared saved results from flash");
    }
}

/**
 * @brief Initialises common attack resources.
 * 
 * Creates attack timeout timer.
 * Registers event loop event handlers.
 */
void attack_init(){
    const esp_timer_create_args_t attack_timeout_args = {
        .callback = &attack_timeout
    };
    ESP_ERROR_CHECK(esp_timer_create(&attack_timeout_args, &attack_timeout_handle));

    ESP_ERROR_CHECK(esp_event_handler_register(WEBSERVER_EVENTS, WEBSERVER_EVENT_ATTACK_REQUEST, &attack_request_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WEBSERVER_EVENTS, WEBSERVER_EVENT_ATTACK_RESET, &attack_reset_handler, NULL));
    
    // Try to load any saved results from previous session
    esp_err_t err = attack_load_results_from_flash();
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Restored previous attack results from flash");
    }
}