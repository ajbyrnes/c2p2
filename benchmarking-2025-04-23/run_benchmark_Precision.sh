#!/bin/bash

# Setup
ALGOS=(0 1 2 3 4)
INTERP_ALGOS=(0 1)

timestamp=$(date +%Y-%m-%dT%H:%M:%S)

csv=Precision_$(uname -n)_${timestamp}.csv
log=Precision_$(uname -n)_${timestamp}.log

# Write header
echo "Compressor,Iterations,DataName,Precision,CompressionLevel,ErrorBoundMode,Algo,InterpAlgo,CompressionTimeMS,DecompressionTimeMS,CompressionRatio,CompressedDataSize,OriginalDataSize" >> $csv

# Iterate over data sizes
PRECISIONS=(1 2 3 4 5 6 7)
for PRECISION in "${PRECISIONS[@]}"
do
    # Run benchmark for bit truncation
    ./benchmark --precision $PRECISION --debug 1 --doTrunk 1 --doSZ 0 >> $csv 2>> $log

    # Run benchmark for SZ3
    for ALGO in "${ALGOS[@]}"
    do
        for INTERP_ALGO in "${INTERP_ALGOS[@]}"
        do
            ./benchmark --precision $PRECISION --debug 1 --szAlgo $ALGO --szInterpAlgo $INTERP_ALGO >> $csv 2>> $log
        done
    done
done