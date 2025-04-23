#!/bin/bash

ALGOS=(0 1 2 3)
INTERP_ALGOS=(0 1)

timestamp=$(date +%Y-%m-%dT%H:%M:%S)

csv="SZ3Sweep_$(uname -n)_${timestamp}.csv"
log="SZ3Sweep_$(uname -n_${timestamp}.log"

echo "Compressor,Iterations,DataName,Precision,CompressionLevel,ErrorBoundMode,Algo,InterpAlgo,CompressionTimeMS,DecompressionTimeMS,CompressionRatio,CompressedDataSize,OriginalDataSize" >> $csv

for ALGO in "${ALGOS[@]}"
do
    for INTERP_ALGO in "${INTERP_ALGOS[@]}"
    do
        ./benchmark --debug 1 --szAlgo $ALGO --szInterpAlgo $INTERP_ALGO >> $csv 2>> $log
    done
done