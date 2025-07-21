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
logfile="../benchmark results/benchmark-SZ3Compressor_${timestamp}.log"
touch "$logfile"

data_dir="../data"

# Small file
# file="${data_dir}/DAOD_PHYSLITE.37019878._000001.pool.root.1"
# ../bin/benchmark-SZ3Compressor --input-file "$file" >> "$logfile" 2>&1

# # Medium file
# file="${data_dir}/DAOD_PHYSLITE.37019878._000022.pool.root.1"
# ../bin/benchmark-SZ3Compressor --input-file "$file" >> "$logfile" 2>&1

# # Big file
# file="${data_dir}/DAOD_PHYSLITE.37019878._000009.pool.root.1"
# ../bin/benchmark-SZ3Compressor --input-file "$file" >> "$logfile" 2>&1

for file in ${data_dir}/*; do
    ../bin/benchmark-SZ3Compressor --input-file "$file" >> "$logfile" 2>&1
done