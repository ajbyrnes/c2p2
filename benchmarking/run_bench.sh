#!/usr/bin/bash

# Setup
ALGOS=(0 1 2 3 4)
INTERP_ALGOS=(0 1)

BRANCHES=(
    # "mcWeight"
    # "met_et_syst"
    # "met_et"
    # "met_phi"
    # "scaleFactor_BTAG"
    # "scaleFactor_ELE"
    # "scaleFactor_LepTRIGGER"
    # "scaleFactor_MUON"
    # "scaleFactor_PILEUP"
    "jet_E"
    "jet_eta"
    "jet_jvt"
    "jet_MV2c10"
    "jet_phi"
    "jet_pt_syst"
    "jet_pt"
    "largeRjet_D2"
    "largeRjet_E"
    "largeRjet_eta"
    "largeRjet_m"
    "largeRjet_phi"
    "largeRjet_pt_syst"
    "largeRjet_pt"
    "largeRjet_tau32"
    "lep_E"
    "lep_eta"
    "lep_etcone20"
    "lep_phi"
    "lep_pt_syst"
    "lep_pt"
    "lep_ptcone30"
    "lep_trackd0pvunbiased"
    "lep_tracksigd0pvunbiased"
    "lep_z0"
)

# Set up output files
RESULTS_DIR="results"
mkdir -p $RESULTS_DIR

timestamp=$(date +%Y-%m-%d_%H-%M-%S)
meta_log="${RESULTS_DIR}/${timestamp}_meta.log"

# Iterate over branches
for branch in "${BRANCHES[@]}"; do
    timestamp=$(date +%Y-%m-%d_%H-%M-%S)
    echo "[$timestamp] Running benchmark for branch: $branch" >> $meta_log

    # Run benchmark for bit truncation
    timestamp=$(date +%Y-%m-%d_%H-%M-%S)
    results_log="${RESULTS_DIR}/${timestamp}_${branch}_trunk.log"
    cmd="./benchmark --debug 1 --doTrunk 1 --doSZ 0 --branchName $branch"
    echo "[$timestamp] Doing $cmd" >> $meta_log
    $cmd > $results_log 2>> $meta_log

    # Run benchmark for SZ3
    for algo in "${ALGOS[@]}"; do
        # Iterate over interpolation algorithms
        for interp_algo in "${INTERP_ALGOS[@]}"; do
            # Run the benchmark script with the current parameters
            timestamp=$(date +%Y-%m-%d_%H-%M-%S)
            results_log="${RESULTS_DIR}/${timestamp}_${branch}_SZ3_${algo}_${interp_algo}.log"
            cmd="./benchmark --debug 1 --doTrunk 0 --doSZ 1 --szAlgo $algo --szInterpAlgo $interp_algo --branchName $branch"
            echo "[$timestamp] Doing $cmd" >> $meta_log
            $cmd > $results_log 2>> $meta_log
        done
    done
done