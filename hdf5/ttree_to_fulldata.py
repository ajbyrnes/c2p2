import os
import uproot
import awkward as ak
import pandas as pd
import numpy as np
import h5py

data_dir = '../134G-data'
data_files = os.listdir(data_dir)
tree_name = "CollectionTree"

# Iterate over all files in the data directory
with h5py.File("/mnt/d/data/fulldata.h5", "a") as out:
    for data_file in data_files:
        print("Processing file:", data_file)
        
        data_file = data_file.split(".")
        data_id = data_file[1]
        file_num = data_file[2].lstrip("_")

        ttree = uproot.open(os.path.join(data_dir, ".".join(data_file)))[tree_name]
        # Select branches to pull from the ROOT TTree
        # branches = [branch for branch in ttree.keys() if 'float' in ttree[branch].typename]        
        branches = [
            "AnalysisJetsAuxDyn.pt",
            "AnalysisJetsAuxDyn.eta",
            "AnalysisJetsAuxDyn.phi",
        ]

        for idx, branch in enumerate(branches):    
            # Three arrays per variable
            # 1. The data itself
            # 2. The event index of each data point
            # i.e. which event is this measurement attached to
            # 3. The local index of each data point
            # i.e. the order in which the measurement was taken
            data = ttree[branch].array()
            event_idx = ak.broadcast_arrays(
                ak.Array(range(len(data))),
                data
            )[0]
            local_idx = ak.local_index(data)
            
            # max_len = ak.max(ak.num(data))

            # Convert data to numpy arrays
            try:
                data = ak.to_numpy(ak.flatten(data))
                event_idx = ak.to_numpy(ak.flatten(event_idx))
                local_idx = ak.to_numpy(ak.flatten(local_idx))
            except Exception as e:
                print("Error converting to numpy:", e)

            dataset_path = '/'.join(branch.split("."))
            grp_name = f"{dataset_path}/{data_id}/{file_num}"
            print(f"Writing datasets {idx + 1}/{len(branches)}: {grp_name}")
            out.require_group(grp_name)
            
            out[grp_name].create_dataset(
                "data", data=data, 
                dtype=np.float32,
                compression="gzip", compression_opts=9, shuffle=True
            )
            out[grp_name].create_dataset(
                "event_index", data=event_idx, dtype=np.int32,
                compression="gzip", compression_opts=9, shuffle=True
            )
            out[grp_name].create_dataset(
                "local_index", data=local_idx, dtype=np.int32,
                compression="gzip", compression_opts=9, shuffle=True
            )
