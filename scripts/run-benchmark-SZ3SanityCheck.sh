#!/bin/bash

# 8 MB = 10 * 1024 * 1024 bytes
# 16 MB = 16 * 1024 * 1024 bytes
# 32 MB = 32 * 1024 * 1024 bytes
# 64 MB = 64 * 1024 * 1024 bytes

# Get timestamp
timestamp=$(date "+%Y-%m-%d %H:%M:%S")

# Create log file
logfile=$timestamp-benchmark-SZ3SanityCheck.log
touch "$logfile"

../bin/benchmark-SZ3SanityCheck --input-file "$file" >> "$logfile" 2>&1