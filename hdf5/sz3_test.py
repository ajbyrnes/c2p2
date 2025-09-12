import h5py
import numpy as np

data = np.random.rand(128, 128, 128).astype(np.float32)

sz3_id = 32024
zstd_id = 32015

with h5py.File("testfile.h5", "w") as f:
    f.create_dataset("random_data", data=data, compression=sz3_id, compression_opts=(4081251088,50528256,16843009,0,16777216,0,4026531840,41023,2147483904,16777216))