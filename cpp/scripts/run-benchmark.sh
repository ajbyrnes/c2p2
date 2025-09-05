#!/bin/bash

# Select benchmark to run
compressor="TruncCompressor"
# compressor="SZ3Compressor"
# compressor="SZ3SanityCheck"

# Set input data directory
data_dir="../13G-data"

# Make 'benchmark results' directory if it doesn't exist
mkdir -p "../benchmark results"

# Create log file
timestamp=$(date "+%Y-%m-%d_%H-%M-%S")
logfile="../benchmark results/benchmark-${compressor}_${timestamp}.log"
touch "$logfile"

# Run the benchmark for each file in the data directory
for file in ${data_dir}/*; do
    ../bin/benchmark-${compressor} --input-file "$file" >> "$logfile" 2>&1
done