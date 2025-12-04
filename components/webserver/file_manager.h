/**
 * @file file_manager.h
 * @brief File manager for SPIFFS-based result persistence
 */

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdint.h>
#include "esp_err.h"

/**
 * @brief File metadata structure for result listings
 */
typedef struct {
    char filename[128];
    uint32_t size;
    uint32_t timestamp;
} file_info_t;

/**
 * @brief Initialize SPIFFS mount point for results storage
 * @return ESP_OK on success
 */
esp_err_t file_manager_init(void);

/**
 * @brief Save PCAP data to SPIFFS with timestamp
 * @param buffer pointer to PCAP data
 * @param size size of PCAP data
 * @return ESP_OK on success
 */
esp_err_t file_manager_save_pcap(const uint8_t *buffer, uint32_t size);

/**
 * @brief Save HCCAPX data to SPIFFS with timestamp
 * @param buffer pointer to HCCAPX data (hccapx_t struct)
 * @param ssid pointer to SSID string
 * @param ssid_len length of SSID
 * @return ESP_OK on success
 */
esp_err_t file_manager_save_hccapx(const uint8_t *buffer, uint32_t size, const uint8_t *ssid, uint32_t ssid_len);

/**
 * @brief Get list of saved result files
 * @param files array to store file info (allocated by caller)
 * @param max_files maximum number of files to return
 * @return number of files found, -1 on error
 */
int file_manager_list_files(file_info_t *files, int max_files);

/**
 * @brief Delete a result file by name
 * @param filename name of file to delete
 * @return ESP_OK on success
 */
esp_err_t file_manager_delete_file(const char *filename);

/**
 * @brief Get file by name and return buffer
 * @param filename name of file to retrieve
 * @param size output parameter for file size
 * @return pointer to file buffer (allocated by function, must be freed by caller), NULL on error
 */
uint8_t *file_manager_get_file(const char *filename, uint32_t *size);

/**
 * @brief Get free space on SPIFFS
 * @return free space in bytes, 0 on error
 */
uint32_t file_manager_get_free_space(void);

/**
 * @brief Get total space on SPIFFS
 * @return total space in bytes, 0 on error
 */
uint32_t file_manager_get_total_space(void);

#endif /* FILE_MANAGER_H */
