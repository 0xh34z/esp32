# Testing Guide

## Overview

This document describes the testing approach for the ESP32 Wi-Fi Penetration Tool project.

## Test Categories

### 1. Unit Tests

Unit tests for individual components can be added under `components/<component>/test/`.

#### Example Structure:
```
components/
  wifi_controller/
    test/
      test_wifi_controller.c
      CMakeLists.txt
```

#### Running Tests:
```bash
# Set up test environment
idf.py -C components/<component>/test build

# Run tests on device
idf.py -C components/<component>/test flash monitor
```

### 2. Integration Tests

Test complete attack workflows on actual hardware.

#### Test Scenarios:
- Management AP startup and connectivity
- AP scanning functionality
- PMKID attack execution
- Handshake capture
- DoS attack execution
- PCAP file generation
- HCCAPX file generation

### 3. Hardware Tests

#### Required Equipment:
- ESP32 development board
- Test router (dedicated for testing)
- Client device (phone/laptop)
- USB serial connection

#### Test Checklist:
- [ ] Management AP starts successfully
- [ ] Web interface accessible
- [ ] AP scanning returns results
- [ ] Attack execution completes
- [ ] File downloads work
- [ ] Reset functionality works
- [ ] Memory doesn't leak over time
- [ ] Device handles errors gracefully

## Writing Tests

### Example Unit Test:

```c
#include "unity.h"
#include "wifi_controller.h"

TEST_CASE("wifi_controller channel validation", "[wifi_controller]")
{
    // Valid channel
    wifictl_set_channel(6);
    TEST_ASSERT_TRUE(true); // Would check actual channel
    
    // Invalid channel (should log error)
    wifictl_set_channel(14);
    wifictl_set_channel(0);
}

TEST_CASE("wifi_controller MAC address operations", "[wifi_controller]")
{
    uint8_t mac[6];
    uint8_t test_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    
    wifictl_get_ap_mac(mac);
    TEST_ASSERT_NOT_NULL(mac);
    
    wifictl_set_ap_mac(test_mac);
    wifictl_restore_ap_mac();
}
```

## Manual Testing Procedures

### 1. Basic Functionality Test

1. Flash firmware to ESP32
2. Power on device
3. Connect to Management AP
4. Access web interface at 192.168.4.1
5. Scan for APs
6. Select target AP
7. Run attack
8. Download results

### 2. Stress Test

1. Run continuous attacks for 1+ hours
2. Monitor for memory leaks
3. Check for crashes or hangs
4. Verify performance doesn't degrade

### 3. Error Handling Test

1. Test with invalid inputs
2. Test network disconnections
3. Test rapid attack start/stop
4. Test with no APs nearby

## Performance Benchmarks

Track these metrics:

- **Memory usage** during attacks
- **Attack success rate** against test AP
- **Time to capture** handshake/PMKID
- **Web interface responsiveness**
- **Power consumption** during operations

## Continuous Integration

GitHub Actions runs automated checks on every push:

- Build verification
- Warning detection  
- Code format checks
- TODO/FIXME tracking

## Test Coverage Goals

Aim for coverage in these areas:

- [ ] Error handling paths
- [ ] Memory allocation/deallocation
- [ ] Input validation
- [ ] State transitions
- [ ] Edge cases

## Debugging Tests

Enable verbose logging:
```c
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
```

Use ESP-IDF monitor:
```bash
idf.py monitor
```

Memory debugging:
```bash
idf.py menuconfig
# Component config → Heap memory debugging
```

## Known Limitations

Document any known test limitations or flaky tests here.

## Contributing Tests

When contributing, please:
1. Add tests for new features
2. Ensure existing tests pass
3. Document test procedures
4. Update this guide if needed
