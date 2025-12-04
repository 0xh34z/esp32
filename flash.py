#!/usr/bin/env python3
"""
Flash helper script for ESP32 Wi-Fi Penetration Tool
Simplifies flashing process with pre-built binaries
"""

import argparse
import os
import sys
import subprocess

# Default flash addresses
BOOTLOADER_ADDR = "0x1000"
PARTITION_TABLE_ADDR = "0x8000"
APP_ADDR = "0x10000"

def find_port():
    """Try to auto-detect serial port"""
    import serial.tools.list_ports
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        return None
    return ports[0].device

def flash_esp32(port, baudrate, bootloader, partition_table, app):
    """Flash ESP32 with specified binaries"""
    
    if not port:
        port = find_port()
        if not port:
            print("Error: Could not detect serial port. Please specify with -p")
            return False
    
    # Check if files exist
    for file in [bootloader, partition_table, app]:
        if not os.path.exists(file):
            print(f"Error: File not found: {file}")
            return False
    
    cmd = [
        "esptool.py",
        "-p", port,
        "-b", str(baudrate),
        "--after", "hard_reset",
        "write_flash",
        "--flash_mode", "dio",
        "--flash_freq", "40m",
        "--flash_size", "detect",
        BOOTLOADER_ADDR, bootloader,
        PARTITION_TABLE_ADDR, partition_table,
        APP_ADDR, app
    ]
    
    print(f"Flashing ESP32 on {port} at {baudrate} baud...")
    print(f"Command: {' '.join(cmd)}")
    
    try:
        subprocess.run(cmd, check=True)
        print("\nFlash successful!")
        return True
    except subprocess.CalledProcessError as e:
        print(f"\nFlash failed: {e}")
        return False
    except FileNotFoundError:
        print("\nError: esptool.py not found. Please install: pip install esptool")
        return False

def main():
    parser = argparse.ArgumentParser(
        description="Flash ESP32 Wi-Fi Penetration Tool"
    )
    parser.add_argument(
        "-p", "--port",
        help="Serial port (e.g., COM3 or /dev/ttyUSB0). Auto-detect if not specified."
    )
    parser.add_argument(
        "-b", "--baudrate",
        type=int,
        default=460800,
        help="Baud rate (default: 460800)"
    )
    parser.add_argument(
        "--build-dir",
        default="build",
        help="Build directory (default: build)"
    )
    
    args = parser.parse_args()
    
    # Construct paths to binaries
    bootloader = os.path.join(args.build_dir, "bootloader", "bootloader.bin")
    partition_table = os.path.join(args.build_dir, "partition_table", "partition-table.bin")
    app = os.path.join(args.build_dir, "esp32-wifi-penetration-tool.bin")
    
    success = flash_esp32(args.port, args.baudrate, bootloader, partition_table, app)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
