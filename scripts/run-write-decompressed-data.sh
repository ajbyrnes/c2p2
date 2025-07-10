#!/bin/bash

max_bytes=$((8 * 1024 * 1024))  # 8 MB

../bin/writeDecompressedData --branch AnalysisJetsAuxDyn.pt --max-bytes $max_bytes > writeDecompressedData.log 2>&1
../bin/writeDecompressedData --branch AnalysisJetsAuxDyn.eta --max-bytes $max_bytes >> writeDecompressedData.log 2>&1
../bin/writeDecompressedData --branch AnalysisJetsAuxDyn.phi --max-bytes $max_bytes >> writeDecompressedData.log 2>&1