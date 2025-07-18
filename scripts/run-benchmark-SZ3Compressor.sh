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
logfile="../benchmark results/${timestamp}_benchmark-SZ3Compressor.log"
touch "$logfile"

data_dir="../data"
file="${data_dir}/DAOD_PHYSLITE.37019878._000001.pool.root.1"
../bin/benchmark-SZ3Compressor --input-file "$file" >> "$logfile" 2>&1
# for file in ${data_dir}/*; do
#     ../bin/benchmark-SZ3Compressor --input-file "$file" >> "$logfile" 2>&1
# done