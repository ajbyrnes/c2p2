import os
import sys

import uproot
    
data_dir = "../data"

print("filename,branch_name,branch_type,compressor,compressed_bytes,uncompressed_bytes,compression_ratio")

# List all files in the directory
for filename in os.listdir(data_dir):
    file = uproot.open(os.path.join(data_dir, filename))
    tree = file["CollectionTree"]
    for branch in tree.branches:
        if not branch.uncompressed_bytes or not branch.compressed_bytes:
            continue
        
        safe_typename = branch.typename.replace(",", ":")

        print(f"{filename},{branch.name},{safe_typename},{branch.compression},{branch.compressed_bytes},{branch.uncompressed_bytes},{branch.compression_ratio}")