import h5py
import hdf5plugin
from functools import partial
import time
import socket
import os

class CompressionBenchmark:
    def __init__(self, source_file, out_file):
        self.source_file = source_file
        self.out_file = out_file
        
    def benchmark(self, dset_name, compressor, compressor_opts):
        """
        Benchmark compression on specified branches of the HDF5 file.
        """        
       # Repack data.h5 to data_<compressor>.h5 with specified compression
    #    os.system("h5repack -f {} -l CHUNK=1024x -i {} -o {}".format(
    #        compressor_opts, self.source_file, self.out_file
    #    ))

def main():
    """
    Main function to run compression benchmark.
    """
    source_file = "/mnt/d/data/fulldata.h5"
    out_file = "/mnt/d/data/compressed.h5"
    branches = [
        "AnalysisJetsAuxDyn/pt",
        # "AnalysisJetsAuxDyn/eta",
        # "AnalysisJetsAuxDyn/phi",
    ]

    benchmark = CompressionBenchmark(source_file, out_file, branches)
    benchmark.benchmark("zstd", "zstd", {'clevel': 5})
    # benchmark.benchmark("sz3", "sz3", {'absolute': 1e-3})
    # benchmark.benchmark("sz3", {'relative': 1e-3})
    
    
if __name__ == "__main__":
    main()