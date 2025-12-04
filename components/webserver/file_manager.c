/**
 * @file file_manager.c
 * @brief File manager implementation for SPIFFS-based result persistence
 */

#include "file_manager.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "file_manager";

#define RESULTS_BASE_PATH "/results"
#define SPIFFS_PARTITION_LABEL "results"

/**
 * @brief Initialize SPIFFS mount point for results storage
 */
esp_err_t file_manager_init(void) {
    ESP_LOGI(TAG, "Initializing SPIFFS for results storage");

    // Check if SPIFFS already mounted
    struct stat st;
    if (stat(RESULTS_BASE_PATH, &st) == 0) {
        ESP_LOGI(TAG, "SPIFFS already mounted at %s", RESULTS_BASE_PATH);
        return ESP_OK;
    }

    // Mount SPIFFS partition
    esp_vfs_spiffs_conf_t conf = {
        .base_path = RESULTS_BASE_PATH,
        .partition_label = SPIFFS_PARTITION_LABEL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "SPIFFS partition not found. Check partition table and sdkconfig.");
        } else if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "SPIFFS already mounted or corrupted. Formatting...");
        } else {
            ESP_LOGE(TAG, "Failed to mount SPIFFS: %s", esp_err_to_name(ret));
        }
        return ret;
    }

    ESP_LOGI(TAG, "SPIFFS mounted successfully at %s", RESULTS_BASE_PATH);

    // Try to create results directory
    mkdir(RESULTS_BASE_PATH, 0755);

    return ESP_OK;
}

/**
 * @brief Generate timestamped filename
 */
static void generate_filename(const char *extension, char *filename, size_t max_len) {
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    snprintf(filename, max_len, "%s/capture_%04d%02d%02d_%02d%02d%02d.%s",
             RESULTS_BASE_PATH,
             timeinfo->tm_year + 1900,
             timeinfo->tm_mon + 1,
             timeinfo->tm_mday,
             timeinfo->tm_hour,
             timeinfo->tm_min,
             timeinfo->tm_sec,
             extension);
}

/**
 * @brief Save PCAP data to SPIFFS with timestamp
 */
esp_err_t file_manager_save_pcap(const uint8_t *buffer, uint32_t size) {
    if (buffer == NULL || size == 0) {
        ESP_LOGE(TAG, "Invalid buffer or size for PCAP save");
        return ESP_ERR_INVALID_ARG;
    }

    char filename[128];
    generate_filename("pcap", filename, sizeof(filename));

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", filename);
        return ESP_FAIL;
    }

    size_t written = fwrite(buffer, 1, size, file);
    fclose(file);

    if (written != size) {
        ESP_LOGE(TAG, "Failed to write complete PCAP file. Written: %zu, Expected: %u", written, size);
        remove(filename);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Successfully saved PCAP file: %s (%u bytes)", filename, size);
    return ESP_OK;
}

/**
 * @brief Save HCCAPX data to SPIFFS with timestamp
 */
esp_err_t file_manager_save_hccapx(const uint8_t *buffer, uint32_t size, const uint8_t *ssid, uint32_t ssid_len) {
    if (buffer == NULL || size == 0) {
        ESP_LOGE(TAG, "Invalid buffer or size for HCCAPX save");
        return ESP_ERR_INVALID_ARG;
    }

    char filename[128];
    generate_filename("hccapx", filename, sizeof(filename));

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", filename);
        return ESP_FAIL;
    }

    size_t written = fwrite(buffer, 1, size, file);
    fclose(file);

    if (written != size) {
        ESP_LOGE(TAG, "Failed to write complete HCCAPX file. Written: %zu, Expected: %u", written, size);
        remove(filename);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Successfully saved HCCAPX file: %s (%u bytes) - SSID: %.*s", 
             filename, size, ssid_len, (char *)ssid);
    return ESP_OK;
}

/**
 * @brief Get list of saved result files
 */
int file_manager_list_files(file_info_t *files, int max_files) {
    if (files == NULL || max_files <= 0) {
        ESP_LOGE(TAG, "Invalid files array or max_files");
        return -1;
    }

    DIR *dir = opendir(RESULTS_BASE_PATH);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Failed to open results directory: %s", RESULTS_BASE_PATH);
        return -1;
    }

    int file_count = 0;
    struct dirent *entry;
    struct stat st;
    char full_path[512];

    while ((entry = readdir(dir)) != NULL && file_count < max_files) {
        if (entry->d_name[0] == '.') {
            continue; // Skip . and ..
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", RESULTS_BASE_PATH, entry->d_name);

        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            strncpy(files[file_count].filename, entry->d_name, sizeof(files[file_count].filename) - 1);
            files[file_count].filename[sizeof(files[file_count].filename) - 1] = '\0';
            files[file_count].size = st.st_size;
            files[file_count].timestamp = st.st_mtime;
            file_count++;
        }
    }

    closedir(dir);
    ESP_LOGD(TAG, "Found %d result files", file_count);
    return file_count;
}

/**
 * @brief Delete a result file by name
 */
esp_err_t file_manager_delete_file(const char *filename) {
    if (filename == NULL) {
        ESP_LOGE(TAG, "Invalid filename");
        return ESP_ERR_INVALID_ARG;
    }

    // Prevent directory traversal attacks
    if (strchr(filename, '/') != NULL) {
        ESP_LOGE(TAG, "Invalid filename contains path separator");
        return ESP_ERR_INVALID_ARG;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", RESULTS_BASE_PATH, filename);

    if (remove(full_path) == 0) {
        ESP_LOGI(TAG, "Successfully deleted file: %s", filename);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to delete file: %s", filename);
        return ESP_FAIL;
    }
}

/**
 * @brief Get file by name and return buffer
 */
uint8_t *file_manager_get_file(const char *filename, uint32_t *size) {
    if (filename == NULL || size == NULL) {
        ESP_LOGE(TAG, "Invalid filename or size pointer");
        return NULL;
    }

    // Prevent directory traversal attacks
    if (strchr(filename, '/') != NULL) {
        ESP_LOGE(TAG, "Invalid filename contains path separator");
        return NULL;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", RESULTS_BASE_PATH, filename);

    FILE *file = fopen(full_path, "rb");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open file: %s", filename);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    uint8_t *buffer = (uint8_t *)malloc(*size);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for file: %s (%u bytes)", filename, *size);
        fclose(file);
        return NULL;
    }

    // Read file
    size_t read = fread(buffer, 1, *size, file);
    fclose(file);

    if (read != *size) {
        ESP_LOGE(TAG, "Failed to read complete file. Read: %zu, Expected: %u", read, *size);
        free(buffer);
        return NULL;
    }

    ESP_LOGI(TAG, "Successfully loaded file: %s (%u bytes)", filename, *size);
    return buffer;
}

/**
 * @brief Get free space on SPIFFS
 */
uint32_t file_manager_get_free_space(void) {
    size_t total = 0, used = 0;
    esp_spiffs_info(SPIFFS_PARTITION_LABEL, &total, &used);
    return (uint32_t)(total - used);
}

/**
 * @brief Get total space on SPIFFS
 */
uint32_t file_manager_get_total_space(void) {
    size_t total = 0, used = 0;
    esp_spiffs_info(SPIFFS_PARTITION_LABEL, &total, &used);
    return (uint32_t)total;
}
