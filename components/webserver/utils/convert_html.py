#!/usr/bin/env python3
"""
Convert HTML file to gzipped C header file for ESP32
"""

import sys
import gzip
import os

def convert_html_to_header(html_file):
    # Read HTML file
    with open(html_file, 'rb') as f:
        html_content = f.read()
    
    # Compress with gzip
    compressed = gzip.compress(html_content, compresslevel=9)
    
    # Get base name
    base_name = os.path.splitext(os.path.basename(html_file))[0]
    output_name = f"page_{base_name}"
    output_file = f"../pages/{output_name}.h"
    upper_name = output_name.upper()
    
    print(f"Converting {html_file}...")
    print(f"Original size: {len(html_content)} bytes")
    print(f"Compressed size: {len(compressed)} bytes")
    print(f"Compression ratio: {100 - (len(compressed) * 100 / len(html_content)):.1f}%")
    
    # Create header content
    header = f"""#ifndef {upper_name}_H
#define {upper_name}_H

// This file was generated from {html_file}
// Original size: {len(html_content)} bytes
// Compressed size: {len(compressed)} bytes

static const unsigned char {output_name}[] = {{
"""
    
    # Add compressed bytes
    for i in range(0, len(compressed), 12):
        line = compressed[i:i+12]
        header += "  " + ", ".join(f"0x{b:02x}" for b in line)
        if i + 12 < len(compressed):
            header += ","
        header += "\n"
    
    header += f"}};";
    header += f"\nstatic const unsigned int {output_name}_len = {len(compressed)};\n"
    header += "\n#endif\n"
    
    # Write output file
    with open(output_file, 'w') as f:
        f.write(header)
    
    print(f"Successfully created {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python convert_html.py <html_file>")
        sys.exit(1)
    
    convert_html_to_header(sys.argv[1])
