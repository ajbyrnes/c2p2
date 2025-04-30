#!/bin/bash

# Setup
ALGOS=(0 1 2 3 4)
INTERP_ALGOS=(0 1)
MINI_BRANCHES=(
"lep_pt" "lep_eta" "lep_phi" "lep_E" "lep_z0"
"lep_ptcone30" "lep_etcone20" "lep_trackd0pvunbiased" "lep_tracksigd0pvunbiased"
"jet_pt" "jet_eta" "jet_phi" "jet_E" "jet_jvt" "jet_MV2c10"
"photon_pt" "photon_eta" "photon_phi" "photo_E"
"photon_ptcone30" "photon_etcone20"
"tau_pt" "tau_eta" "tau_phi" "tau_E" "tau_BDTid" 
"lep_pt_syst" "jet_pt_syst" "photon_pt_syst" "tau_pt_syst"
"largeRjet_pt" "largeRjet_eta" "largeRjet_phi" "largeRjet_E" "largeRjet_m"
"largeRjet_truthMatched" "largeRjet_D2" "largeRjet_tau32" "largeRjet_pt_syst"
)

timestamp=$(date +%Y-%m-%dT%H:%M:%S)

csv=MiniBranches_$(uname -n)_${timestamp}.csv
log=MiniBranches_$(uname -n)_${timestamp}.log

# Write header
echo "Compressor,Iterations,DataName,Precision,CompressionLevel,ErrorBoundMode,Algo,InterpAlgo,CompressionTimeMS,DecompressionTimeMS,CompressionRatio,CompressedDataSize,OriginalDataSize" >> $csv

# Iterate over branches
for BRANCH in "${MINI_BRANCHES[@]}"
do
    # Run benchmark for bit truncation
    ./benchmark --debug 1 --sortData 1 --doTrunk 1 --doSZ 0 --branchName $BRANCH >> $csv 2>> $log
    
    # Run benchmark for SZ3
    for ALGO in "${ALGOS[@]}"
    do
        for INTERP_ALGO in "${INTERP_ALGOS[@]}"
        do
            ./benchmark --debug 1 --sortData 1 --szAlgo $ALGO --szInterpAlgo $INTERP_ALGO --branchName $BRANCH >> $csv 2>> $log
        done
    done
done