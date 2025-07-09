#!/bin/bash

../bin/benchmark-TruncCompressor --branch AnalysisJetsAuxDyn.pt --iterations 5 > benchmark-TruncCompressor.log
../bin/benchmark-TruncCompressor --branch AnalysisJetsAuxDyn.eta --iterations 5 >> benchmark-TruncCompressor.log
../bin/benchmark-TruncCompressor --branch AnalysisJetsAuxDyn.phi --iterations 5 >> benchmark-TruncCompressor.log