# ESP32 Wi-Fi Penetration Tool

[![Build Status](https://github.com/0xh34z/esp32/workflows/Build%20ESP32%20Project/badge.svg)](https://github.com/0xh34z/esp32/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This project introduces an universal tool for ESP32 platform for implementing various Wi-Fi attacks. It provides some common functionality that is commonly used in Wi-Fi attacks and makes implementing new attacks a bit simpler. It also includes Wi-Fi attacks itself like capturing PMKIDs from handshakes, or handshakes themselves by different methods like starting rogue duplicated AP or sending deauthentication frames directly, etc...

Obviously cracking is not part of this project, as ESP32 is not sufficient to crack hashes in effective way. The rest can be done on this small, cheap, low-power SoC.

<p align="center">
    <img src="doc/images/logo.png" alt="Logo">
</p>

## ⚠️ Legal Disclaimer

**This tool is for educational and authorized testing purposes ONLY.** Using this tool against networks you don't own or have explicit permission to test is illegal. See [SECURITY.md](SECURITY.md) for more information.

## Features
- **PMKID capture**
- **WPA/WPA2 handshake capture** and parsing
- **Deauthentication attacks** using various methods
- **Denial of Service attacks**
- Formatting captured traffic into **PCAP format**
- Parsing captured handshakes into **HCCAPX file** ready to be cracked by Hashcat
- Passive handshake sniffing
- Easily extensible framework for new attacks implementations
- Management AP for easy configuration on the go using smartphone for example
- And more...

### Demo video
[![Demonstration Youtube video](https://img.youtube.com/vi/9I3BxRu86GE/0.jpg)](https://www.youtube.com/watch?v=9I3BxRu86GE)

## What's New in v1.0

- ✨ **Enhanced error handling** throughout the codebase
- 🔒 **Improved security** with input validation
- 📊 **Debug utilities** for memory tracking and troubleshooting  
- 🤖 **CI/CD pipeline** with GitHub Actions
- 📝 **Comprehensive documentation** (CONTRIBUTING.md, SECURITY.md, TESTING.md)
- 🔧 **Better memory management** to prevent leaks
- 📦 **Version tracking** with build information

See [CHANGELOG.md](CHANGELOG.md) for full details.

## Quick Start

### Prerequisites
- ESP-IDF 4.1+ (tested on commit `5ef1b390`)
- ESP32 DevKit or compatible board
- USB cable for flashing
- Python 3.7+ (for flashing script)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/0xh34z/esp32.git
   cd esp32
   ```

2. **Set up ESP-IDF**
   ```bash
   # Follow ESP-IDF installation guide
   # https://docs.espressif.com/projects/esp-idf/en/latest/get-started/
   ```

3. **Build the project**
   ```bash
   idf.py build
   ```

4. **Flash to ESP32**
   ```bash
   # Using idf.py (recommended)
   idf.py -p /dev/ttyUSB0 flash monitor
   
   # Or using the helper script
   python flash.py -p /dev/ttyUSB0
   
   # On Windows
   python flash.py -p COM3
   ```

## Usage
1. [Build](#Build) and [flash](#Flash) project onto ESP32 (DevKit or module)
1. Power ESP32
1. Management AP is started automatically after boot
1. Connect to this AP\
By default: 
*SSID:* `ManagementAP` and *password:* `mgmtadmin`
1. In browser open `192.168.4.1` and you should see a web client to configure and control tool like this:

    ![Web client UI](doc/images/ui-config.png)

## Configuration

### Management AP Settings

Configure via `idf.py menuconfig` under "ESP32 Wi-Fi Penetration Tool Configuration":

- **SSID**: Default `ManagementAP`
- **Password**: Default `mgmtadmin` (min 8 chars)
- **Max Connections**: Default 4

Or edit `sdkconfig.defaults` directly.

### Build Options

```bash
idf.py menuconfig
# Component config → ESP32-specific
```

## Build
This project is currently developed using ESP-IDF 4.1 (commit `5ef1b390026270503634ac3ec9f1ec2e364e23b2`). It may be broken on newer version.

Project can be built in the usual ESP-IDF way:

```shell
idf.py build
```

Legacy method using `make` is not supported by this project.

## Flash
If you have setup ESP-IDF, the easiest way is to use `idf.py flash`.

In case you don't want to setup whole ESP-IDF, you can use pre-build binaries included in [`build/`](build/) and flash them using [`esptool.py`](https://github.com/espressif/esptool) (requires Python).

Example command (follow instructions in [esptool repo](https://github.com/espressif/esptool)):
```
esptool.py -p /dev/ttyS5 -b 115200 --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x8000 build/partition_table/partition-table.bin 0x1000 build/bootloader/bootloader.bin 0x10000 build/esp32-wifi-penetration-tool.bin
```

On Windows you can use official [Flash Download Tool](https://www.espressif.com/en/support/download/other-tools).

## Troubleshooting

### Common Issues

**"Failed to connect to ESP32"**
- Check USB cable and drivers
- Try different baud rate: `-b 115200`
- Press and hold BOOT button during flash

**"Not enough memory"**
- Normal - ESP32 has limited RAM
- Check memory stats in logs
- Reduce buffer sizes in menuconfig if needed

**Web interface not accessible**
- Verify connected to Management AP
- Check IP: should be `192.168.4.1`
- Try different browser
- Check ESP32 logs for errors

**Attack not completing**
- Increase timeout value
- Ensure target AP is in range
- Check target AP uses WPA/WPA2
- Monitor ESP32 logs for errors

### Debug Mode

Enable verbose logging:
```bash
idf.py menuconfig
# Component config → Log output → Default log verbosity → Verbose
```

View logs:
```bash
idf.py monitor
```

See [TESTING.md](TESTING.md) for more troubleshooting steps.

## Documentation

### Project Documentation
- [CHANGELOG.md](CHANGELOG.md) - Version history and changes
- [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute
- [SECURITY.md](SECURITY.md) - Security policy and responsible use
- [TESTING.md](TESTING.md) - Testing procedures and guidelines

### Wi-Fi attacks
Attacks implementations in this project are described in [main component README](main/). Theory behind these attacks is located in [doc/ATTACKS_THEORY.md](doc/ATTACKS_THEORY.md)
### API reference
This project uses Doxygen notation for documenting components API and implementation. Doxyfile is included so if you want to generate API reference, just run `doxygen` from root directory. It will generate HTML API reference into `doc/api/html`.

### Components
This project consists of multiple components, that can be reused in other projects. Each component has it's own README with detailed description. Here comes brief description of components:

- [**Main**](main) component is entry point for this project. All neccessary initialisation steps are done here. Management AP is started and the control is handed to webserver.
- [**Wifi Controller**](components/wifi_controller) component wraps all Wi-Fi related operations. It's used to start AP, connect as STA, scan nearby APs etc. 
- [**Webserver**](components/webserver) component provides web UI to configure attacks. It expects that AP is started and no additional security features like SSL encryption are enabled.
- [**Wi-Fi Stack Libraries Bypasser**](components/wsl_bypasser) component bypasses Wi-Fi Stack Libraries restriction to send some types of arbitrary 802.11 frames.
- [**Frame Analyzer**](components/frame_analyzer) component processes captured frames and provides parsing functionality to other components.
- [**PCAP Serializer**](components/pcap_serializer) component serializes captured frames into PCAP binary format and provides it to other components (mostly for webserver/UI)
- [**HCCAPX Serializer**](components/hccapx_serializer) component serializes captured frames into HCCAPX binary format and provides it to other components (mostly for webserver/UI)

### Further reading
* [Academic paper about this project (PDF)](https://excel.fit.vutbr.cz/submissions/2021/048/48.pdf)

## Hardware 
This project was mostly build and tested on **ESP32-DEVKITC-32E**
but there should not be any differences for any **ESP32-WROOM-32** modules.

<p align="center">
    <img src="doc/images/soucastky_8b.png" alt="Hw components" width="400">
</p>

On the following pictures you can see a battery (Li-Pol accumulator) powered ESP32 DevKitC using following hardware:
- **ESP32-DEVKITC-32E** (cost 213 CZK/8.2 EUR/9.6 USD)
- 220mAh Li-Pol 3.7V accumulator (weights ±5g, cost 77 CZK/3 EUR/3.5 USD)
- MCP1702-3302ET step-down 3.3V voltage regulator (cost 11 CZK/0.42 EUR/0.50 USD)
- Czech 5-koruna coin for scale (weights 4.8g, diameter 23 mm, cost 0.19 EUR/0.23 USD)
<p align="center">
    <img src="doc/images/mini.jpg" alt="Hw components" width="300">
    <img src="doc/images/mini2.jpg" alt="Hw components" width="300">
</p>

Altogether (without coin) this setup weights around 17g. This can be further downsized by using smaller Li-Pol accumulator and using ESP32-WROOM-32 modul directly instead of whole dev board.

This setup cost me around 300 CZK (± 11.50 EUR/13.50 USD). Using the modul directly that costs around 80 CZK (± 3 EUR/3.5 USD) we can get to price of 160 CZK (± 6.5 EUR/7.5 USD) which makes this tool really cheap and available to almost everybody.

### Power consumption
Based on experimental measurements, ESP32 consumes around 100mA during attack executions.

## Performance

### Benchmarks
- **PMKID Capture**: ~10-30 seconds (depends on target AP)
- **Handshake Capture**: ~30-120 seconds (requires client activity)
- **Memory Usage**: ~150KB heap during attacks
- **Power Consumption**: ~100mA @ 3.3V during operation

### Limitations
- Limited to 2.4GHz networks (ESP32 hardware limitation)
- Cannot crack passwords (use Hashcat on PC)
- Range limited to ESP32 antenna (~50m typical)
- Single-channel operation during attacks

## Similar projects
* [GANESH-ICMC/esp32-deauther](https://github.com/GANESH-ICMC/esp32-deauther)
* [SpacehuhnTech/esp8266_deauther](https://github.com/SpacehuhnTech/esp8266_deauther)
* [justcallmekoko/ESP32Marauder](https://github.com/justcallmekoko/ESP32Marauder)
* [EParisot/esp32-network-toolbox](https://www.tindie.com/products/klhnikov/esp32-network-toolbox/)
* [Jeija/esp32free80211](https://github.com/Jeija/esp32free80211)

## FAQ

**Q: Is this legal?**
A: Only when used on networks you own or have written permission to test. See [SECURITY.md](SECURITY.md).

**Q: Can it crack WPA passwords?**
A: No. It captures handshakes/PMKIDs which must be cracked offline using tools like Hashcat.

**Q: Does it work on 5GHz networks?**
A: No. ESP32 only supports 2.4GHz.

**Q: What's the range?**
A: Typically 30-50 meters, similar to regular Wi-Fi range.

**Q: Can I add external antenna?**
A: Yes, if your ESP32 board supports it (check your specific model).

**Q: Why does my attack fail?**
A: Check: target in range, using WPA/WPA2, increase timeout, check logs.

## Contributing
Feel free to contribute. Don't hestitate to refactor current code base. Please stick to Doxygen notation when commenting new functions and files. This project is mainly build for educational and demonstration purposes, so verbose documentation is welcome.

## Disclaimer
This project demonstrates vulnerabilities of Wi-Fi networks and its underlaying 802.11 standard and how ESP32 platform can be utilised to attack on those vulnerable spots. Use responsibly against networks you have permission to attack on.

## License
Even though this project is licensed under MIT license (see [LICENSE](LICENSE) file for details), don't be shy or greedy and share your work.