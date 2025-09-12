import h5py
import hdf5plugin
import numpy as np
import os

data = np.random.rand(1000, 1000)

print(hdf5plugin.Zfp)
print(hdf5plugin.Zfp(precision=16))

print(os.env("HDF5_PLUGIN_PATH"))

with h5py.File('test.h5', 'w') as f:
    f.create_dataset(
        'random_data',
        data=data,
        compression=hdf5plugin.Zfp(precision=10),
        chunks=True
    )
