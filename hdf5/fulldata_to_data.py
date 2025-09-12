import h5py
from functools import partial

def find_data(name, branch, dset_list):
    """
    Find datasets at locations branch/.../data.
    """
    if name.startswith(branch) and name.endswith('data') and '37019878' in name:
        dset_list[branch].append(name)

def main():
    """
    Main function to run compression benchmark.
    """
    source_file = "/mnt/d/data/fulldata.h5"
    out_file = "/mnt/d/data/data.h5"
    branches = [
        "AnalysisJetsAuxDyn/pt",
        "AnalysisJetsAuxDyn/eta",
        "AnalysisJetsAuxDyn/phi",
    ]

    datasets = { branch: [] for branch in branches }
    print(f'Searching for datasets in {source_file}')
    with h5py.File(source_file, 'r') as f:
        for branch in branches:
            f.visit(partial(find_data, branch=branch, dset_list=datasets))
    
    # ATLAS Open Data is distributed as a series of .root files in the DAOD_PHYSLITE format,
    # with the .root files grouped by a unique run number.
    # Here we will merge data from multiple files and runs into a single HDF5 file,
    # where each dataset corresponds to a ROOT TTree branch.
    with h5py.File(source_file, 'r') as src, h5py.File(out_file, 'w-') as dst:
        for branch in branches:
            # Get combined size of all datasets for this branch
            total_size = sum(src[dset].shape[0] for dset in datasets[branch])
            print(f'Creating dataset for branch {branch} with total size {total_size}')
            
            # Create empty dataset
            combined_dset_name = branch + "/data"
            combined_dset = dst.create_dataset(combined_dset_name, shape=(total_size,), dtype='f4', chunks=True)
            
            # Fill combined dataset with data from each individual dataset
            offset = 0
            for dset in datasets[branch]:
                print(f'Copying data from {dset} to {combined_dset_name} at offset {offset}')
                data = src[dset][:]
                combined_dset[offset:offset + data.shape[0]] = data
                offset += data.shape[0]
            print(f'Finished combining datasets for branch {branch}')
            
            
if __name__ == "__main__":
    main()