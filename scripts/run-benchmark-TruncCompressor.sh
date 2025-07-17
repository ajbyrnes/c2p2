#!/bin/bash

# 8 MB = 10 * 1024 * 1024 bytes
# 16 MB = 16 * 1024 * 1024 bytes
# 32 MB = 32 * 1024 * 1024 bytes
# 64 MB = 64 * 1024 * 1024 bytes

# Make 'benchmark results' directory if it doesn't exist
mkdir -p "../benchmark results"

# Get timestamp
timestamp=$(date "+%Y-%m-%d_%H-%M-%S")

# Create log file
logfile="../benchmark results/${timestamp}_benchmark-TruncCompressor.log"
touch "$logfile"

# Run benchmark
data_dir="../data"
for file in ${data_dir}/*; do
    ../bin/benchmark-TruncCompressor --input-file "$file" >> "$logfile"
done