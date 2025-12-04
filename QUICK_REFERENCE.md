# Quick Reference Guide

## One-Page Cheat Sheet for ESP32 Wi-Fi Penetration Tool

### 🚀 Quick Start
```bash
# 1. Build
idf.py build

# 2. Flash
python flash.py -p COM3        # Windows
python flash.py -p /dev/ttyUSB0  # Linux

# 3. Connect
SSID: ManagementAP
Pass: mgmtadmin
URL: http://192.168.4.1
```

### 📡 Default Configuration
| Setting | Default Value |
|---------|--------------|
| Management SSID | ManagementAP |
| Management Password | mgmtadmin |
| Management IP | 192.168.4.1 |
| Max Connections | 4 |
| Attack Timeout | 60s |

### 🔧 Common Commands
```bash
# Build project
idf.py build

# Flash and monitor
idf.py -p COM3 flash monitor

# Clean build
idf.py fullclean

# Configuration menu
idf.py menuconfig

# Monitor logs only
idf.py monitor

# Exit monitor
Ctrl+]
```

### 🎯 Attack Types
1. **PMKID** - Capture PMKID from AP
2. **Handshake** - Capture 4-way handshake
3. **DoS** - Denial of Service attack
4. **Passive** - Passive monitoring

### 📥 Output Formats
- **PCAP** - Wireshark compatible
- **HCCAPX** - Hashcat compatible

### 🐛 Debugging
```bash
# Enable verbose logs
idf.py menuconfig
# → Component config → Log output → Verbose

# View memory stats
# Check serial output after startup

# Common issues:
# - Flash fails: Try -b 115200
# - No connection: Press BOOT button
# - Web not loading: Check WiFi connection
```

### 📝 File Locations
```
build/               - Compiled binaries
components/          - Reusable components
main/                - Main application
doc/                 - Documentation
.github/workflows/   - CI/CD
```

### 🔐 Security Notes
- ⚠️ Legal use only - own networks or with permission
- 🔒 Captures don't include passwords
- 📡 2.4GHz only (ESP32 limitation)
- 📏 Range: ~30-50m typical

### 🌐 Web Interface Endpoints
| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/` | GET | Web UI |
| `/ap-list` | GET | Scan APs |
| `/run-attack` | POST | Start attack |
| `/status` | GET | Attack status |
| `/reset` | HEAD | Reset state |
| `/capture.pcap` | GET | Download PCAP |
| `/capture.hccapx` | GET | Download HCCAPX |

### 💾 Memory Usage
- Typical: ~150KB heap during attacks
- Monitor: Check logs for memory stats
- Debug: Enable heap tracing in menuconfig

### ⚡ Power Consumption
- Idle: ~80mA
- Attack: ~100mA
- Voltage: 3.3V
- Battery: 220mAh lasts ~2 hours

### 📚 Documentation Links
- [README.md](README.md) - Full documentation
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute
- [SECURITY.md](SECURITY.md) - Security policy
- [TESTING.md](TESTING.md) - Testing guide
- [CHANGELOG.md](CHANGELOG.md) - Version history

### 🆘 Troubleshooting Quick Fixes
| Problem | Solution |
|---------|----------|
| Can't flash | Check USB cable, try BOOT button |
| No WiFi AP | Wait 30s, check logs, reboot |
| Web not loading | Verify IP, try different browser |
| Attack fails | Increase timeout, check range |
| Out of memory | Normal - limited RAM |
| Build fails | Update ESP-IDF, clean build |

### 🔄 Update Process
```bash
# Pull latest
git pull origin master

# Clean old build
idf.py fullclean

# Rebuild
idf.py build

# Reflash
python flash.py -p COM3
```

### 📊 Status LED (if available)
- Solid: Normal operation
- Blinking: Attack in progress
- Off: Check power/connection

### 🛠️ Custom Configuration
```bash
# Edit config
idf.py menuconfig
# → ESP32 Wi-Fi Penetration Tool Configuration

# Or edit directly
nano sdkconfig

# Apply changes
idf.py build flash
```

### 📱 Client Compatibility
- ✅ Chrome/Edge (recommended)
- ✅ Firefox
- ✅ Safari
- ✅ Mobile browsers
- ❌ Very old browsers

### 🏗️ Building from Source
```bash
# Prerequisites
# - ESP-IDF 4.1+
# - Python 3.7+
# - Git

# Clone
git clone <repo-url>
cd esp32

# Build
idf.py build

# Done!
```

### 🎓 Learning Resources
- ESP-IDF Docs: https://docs.espressif.com
- 802.11 Standard: doc/ATTACKS_THEORY.md
- Similar Projects: See README.md

### ⚖️ Legal Reminders
✅ Own networks
✅ Written permission
✅ Educational purposes
❌ Unauthorized access
❌ Public networks
❌ Any illegal activity

---
**Version:** 1.0.0
**Last Updated:** 2025-12-03
**License:** MIT
