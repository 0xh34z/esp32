/**
 * @file debug_utils.h
 * @author risinek (risinek@gmail.com)
 * @date 2025-12-03
 * @copyright Copyright (c) 2025
 * 
 * @brief Debug utilities for ESP32 Wi-Fi Penetration Tool
 */

#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "esp_log.h"

/**
 * @brief Print memory statistics
 * 
 * Logs current heap usage, largest free block, and minimum free size
 */
void debug_print_memory_stats(void);

/**
 * @brief Print MAC address in readable format
 * 
 * @param tag Log tag to use
 * @param label Label for the MAC address
 * @param mac Pointer to 6-byte MAC address
 */
void debug_print_mac(const char *tag, const char *label, const uint8_t *mac);

/**
 * @brief Hexdump utility for debugging binary data
 * 
 * @param tag Log tag to use
 * @param label Label for the data
 * @param data Pointer to data buffer
 * @param len Length of data
 */
void debug_hexdump(const char *tag, const char *label, const uint8_t *data, size_t len);

/**
 * @brief Check for memory leaks since last check
 * 
 * @param tag Log tag to use
 * @param checkpoint_name Name for this checkpoint
 */
void debug_check_memory_leak(const char *tag, const char *checkpoint_name);

#endif // DEBUG_UTILS_H
