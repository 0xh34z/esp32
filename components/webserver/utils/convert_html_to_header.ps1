# PowerShell script to convert HTML to C header file
# Usage: .\convert_html_to_header.ps1 index.html

param(
    [Parameter(Mandatory=$true)]
    [string]$InputFile
)

if (-not (Test-Path $InputFile)) {
    Write-Error "File not found: $InputFile"
    exit 1
}

# Get base name without extension
$baseName = [System.IO.Path]::GetFileNameWithoutExtension($InputFile)
$outputName = "page_$baseName"
$outputFile = "$outputName.h"
$upperName = $outputName.ToUpper()

Write-Host "Converting $InputFile to $outputFile..."

# Read and gzip the HTML content
$htmlContent = [System.IO.File]::ReadAllBytes($InputFile)
$memoryStream = New-Object System.IO.MemoryStream
$gzipStream = New-Object System.IO.Compression.GZipStream($memoryStream, [System.IO.Compression.CompressionMode]::Compress)
$gzipStream.Write($htmlContent, 0, $htmlContent.Length)
$gzipStream.Close()
$compressedBytes = $memoryStream.ToArray()
$memoryStream.Close()

Write-Host "Original size: $($htmlContent.Length) bytes"
Write-Host "Compressed size: $($compressedBytes.Length) bytes"

# Create header file
$header = @"
#ifndef ${upperName}_H
#define ${upperName}_H

// This file was generated from $InputFile
// Original size: $($htmlContent.Length) bytes
// Compressed size: $($compressedBytes.Length) bytes

"@

# Convert bytes to C array
$header += "static const unsigned char ${outputName}[] = {`n"

$lineLength = 0
for ($i = 0; $i -lt $compressedBytes.Length; $i++) {
    if ($lineLength -eq 0) {
        $header += "  "
    }
    
    $header += "0x{0:x2}" -f $compressedBytes[$i]
    
    if ($i -lt $compressedBytes.Length - 1) {
        $header += ", "
        $lineLength++
        
        if ($lineLength -ge 12) {
            $header += "`n"
            $lineLength = 0
        }
    }
}

$header += "`n};`n"
$header += "static const unsigned int ${outputName}_len = $($compressedBytes.Length);`n"
$header += "`n#endif`n"

# Write to file
$outputPath = Join-Path "..\pages" $outputFile
[System.IO.File]::WriteAllText($outputPath, $header)

Write-Host "Successfully created $outputPath"
