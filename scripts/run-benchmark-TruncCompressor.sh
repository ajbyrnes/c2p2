#!/bin/bash

# 8 MB = 10 * 1024 * 1024 bytes
# 16 MB = 16 * 1024 * 1024 bytes
# 32 MB = 32 * 1024 * 1024 bytes
# 64 MB = 64 * 1024 * 1024 bytes
# touch benchmark-TruncCompressor.log

# for max_bytes in 8 16 32 64;
# do
#     max_bytes=$((max_bytes * 1024 * 1024))

#     ../bin/benchmark-TruncCompressor --max-bytes $max_bytes --branch AnalysisJetsAuxDyn.pt >> benchmark-TruncCompressor.log
#     ../bin/benchmark-TruncCompressor --max-bytes $max_bytes --branch AnalysisJetsAuxDyn.eta >> benchmark-TruncCompressor.log
#     ../bin/benchmark-TruncCompressor --max-bytes $max_bytes --branch AnalysisJetsAuxDyn.phi >> benchmark-TruncCompressor.log
# done

data_dir="../data"

for file in ${data_dir}/*; do
    ../bin/benchmark-TruncCompressor --input-file "$file" >> benchmark-TruncCompressor.log
done