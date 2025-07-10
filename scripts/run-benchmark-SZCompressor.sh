#!/bin/bash
# 8 MB = 10 * 1024 * 1024 bytes
# 16 MB = 16 * 1024 * 1024 bytes
# 32 MB = 32 * 1024 * 1024 bytes
# 64 MB = 64 * 1024 * 1024 bytes
touch benchmark-SZ3Compressor.log

for max_bytes in 8 16 32 64;
do
    max_bytes=$((max_bytes * 1024 * 1024))

    ../bin/benchmark-SZ3Compressor --max-bytes $max_bytes --branch AnalysisJetsAuxDyn.pt >> benchmark-SZ3Compressor.log 2>&1
    ../bin/benchmark-SZ3Compressor --max-bytes $max_bytes --branch AnalysisJetsAuxDyn.eta >> benchmark-SZ3Compressor.log 2>&1
    ../bin/benchmark-SZ3Compressor --max-bytes $max_bytes --branch AnalysisJetsAuxDyn.phi >> benchmark-SZ3Compressor.log 2>&1
done

