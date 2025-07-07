#! /home/ajbyrnes/miniforge3/envs/env/bin/python3

import os
import sys

import uproot

if len(sys.argv) <= 1:
    print("Usage: get_branch_compression_info.py <data_dir>")
    sys.exit(1)
    
data_dir = sys.argv[1]

# List all files in the directory
for filename in os.listdir(data_dir):
    file = uproot.open(os.path.join(data_dir, filename))
    tree = file["CollectionTree"]
    for branch in tree.branches:
        if not branch.uncompressed_bytes or not branch.compressed_bytes:
            continue
        
        print(f"{branch.name},{branch.typename},{branch.compression},{branch.compressed_bytes},{branch.uncompressed_bytes},{branch.compression_ratio}")