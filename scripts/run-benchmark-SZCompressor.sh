#!/bin/bash

../bin/benchmark-SZ3Compressor --branch AnalysisJetsAuxDyn.pt > benchmark-SZ3Compressor.log 2>&1
../bin/benchmark-SZ3Compressor --branch AnalysisJetsAuxDyn.eta >> benchmark-SZ3Compressor.log 2>&1
../bin/benchmark-SZ3Compressor --branch AnalysisJetsAuxDyn.phi >> benchmark-SZ3Compressor.log 2>&1