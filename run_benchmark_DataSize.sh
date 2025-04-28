#!/bin/bash

# Setup
ALGOS=(0 1 2 3 4)
INTERP_ALGOS=(0 1)

timestamp=$(date +%Y-%m-%dT%H:%M:%S)

csv=DataSize_$(uname -n)_${timestamp}.csv
log=DataSize_$(uname -n)_${timestamp}.log

# Write header
echo "Compressor,Iterations,DataName,Precision,CompressionLevel,ErrorBoundMode,Algo,InterpAlgo,CompressionTimeMS,DecompressionTimeMS,CompressionRatio,CompressedDataSize,OriginalDataSize" >> $csv

# Iterate over data sizes
DATASIZES=(0.1 0.5 1 10 100)
for DATASIZE in "${DATASIZES[@]}"
do
    # Run benchmark for bit truncation
    ./benchmark --dataMB $DATASIZE --debug 1 --doTrunk 1 --doSZ 0 >> $csv 2>> $log

    # Run benchmark for SZ3
    for ALGO in "${ALGOS[@]}"
    do
        for INTERP_ALGO in "${INTERP_ALGOS[@]}"
        do
            ./benchmark --dataMB $DATASIZE --debug 1 --szAlgo $ALGO --szInterpAlgo $INTERP_ALGO >> $csv 2>> $log
        done
    done
done