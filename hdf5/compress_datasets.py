import h5py
import hdf5plugin
from functools import partial
import time
import datetime
import socket
import os
from tqdm import tqdm
import sys

def find_data(name, branch, dset_list):
    """
    Find datasets at locations branch/.../data.
    """
    if name.startswith(branch) and name.endswith('data'):
        dset_list.append(name)


class CompressionBenchmark:
    def __init__(self, source_file, out_file, branches):
        self.source_file = source_file
        self.out_file = out_file
        self.branches = branches
        
        # Collect list of datasets to be compressed
        print("Finding datasets...")
        self.datasets = []
        with h5py.File(self.source_file, 'r') as f:
            for branch in branches:
                f.visit(partial(find_data, branch=branch, dset_list=self.datasets))
        
        print(f"Found {len(self.datasets)} datasets that match query.")
    
    
    def compress(self, compressor_settings, raw_plugin=True, rerun=False, probe=False):
        """
        Benchmark compression on specified branches of the HDF5 file.
        """        
        with h5py.File(self.source_file, 'r') as src, h5py.File(self.out_file, 'a') as dst:
            for src_dset_name in self.datasets:
                # Print chunk size
                src_dset = src[src_dset_name]
                src_dset_chunk = src_dset.chunks
                
                # Create new dataset with desired compressor settings
                dst_grp_name = src_dset_name[:-4] + compressor_settings['compressor']
                dst_dset_name = dst_grp_name + "/" + compressor_settings['dst_name']
                    
                # Check if dataset already exists, and if we want to overwrite it
                if dst_dset_name in dst:
                    if not rerun:
                        print(f"Dataset {dst_dset_name} already exists, skipping...")
                        continue
                    else:
                        print(f"Dataset {dst_dset_name} already exists, overwriting...")
                        del dst[dst_dset_name]
                    
                dst.require_group(dst_grp_name)
                
                if raw_plugin:
                    dst_dataset = dst.create_dataset(
                        dst_dset_name, shape=src_dset.shape, dtype=src_dset.dtype,
                        chunks=min(compressor_settings['chunk_size'], src_dset.shape[0]),
                        compression=compressor_settings['id'],
                        compression_opts=compressor_settings['options']['tuple']
                    )
                else:
                    dst_dataset = dst.create_dataset(
                        dst_dset_name, shape=src_dset.shape, dtype=src_dset.dtype,
                        chunks=min(compressor_settings['chunk_size'], src_dset.shape[0]),
                        compression=compressor_settings['obj'](**compressor_settings['options'])
                    )
                
                if probe:
                    # Attempt to write a single chunk to test if compression settings are valid
                    print(f"Probing compression settings for {dst_dset_name}...")
                    end = min(compressor_settings['chunk_size'], src_dset.shape[0])
                    dst_dataset[0:end] = src_dset[0:end]
                    # Flush file, stdout
                    dst.flush()
                    sys.stdout.flush()
                    # End probe
                    del dst[dst_dset_name]
                    print(f"Probe successful for {dst_dset_name}, skipping actual compression...")
                else:
                    # Copy data from source to destination dataset, by 32 KB chunks
                    # Add progress bar
                    chunk_step = compressor_settings['chunk_size']
                    for i in tqdm(range(0, src_dset.shape[0], chunk_step), total=(src_dset.shape[0] + chunk_step - 1) // chunk_step, unit='chunk', desc=dst_dset_name, file=sys.stdout):              
                        end = min(i + chunk_step, src_dset.shape[0])
                        dst_dataset[i:end] = src_dset[i:end]
                        
                    # Save compression results as attributes on compressed dataset
                    dst[dst_dset_name].attrs["host"] = socket.gethostname()
                    dst[dst_dset_name].attrs["timestamp"] = str(datetime.datetime.now())
                    
                    dst[dst_dset_name].attrs["compression"] = compressor_settings['compressor']
                    for k, v in compressor_settings["options"].items():
                        if k == 'tuple': 
                            continue
                        dst[dst_dset_name].attrs[f"compression_option_{k}"] = v
                    
                    dst[dst_dset_name].attrs["time_unit"] = "nanoseconds"
                    dst[dst_dset_name].attrs["original_size"] = src_dset.id.get_storage_size()
                    dst[dst_dset_name].attrs["compressed_size"] = dst[dst_dset_name].id.get_storage_size()
                    dst[dst_dset_name].attrs["size_unit"] = "bytes"
                    
                print("----------\n")


def get_sz3_tuple(options):
    # Modify sz3.config
    if options['CmprAlgo'] == 'ALGO_NO_PRED':
        options['CmprAlgo'] = 'ALGO_LORENZO_REG'
        options['Lorenzo'] = 'No'
        options['Lorenzo2ndOrder'] = 'No'
        options['Regression'] = 'No'
        options['Regression2ndOrder'] = 'No'

    # Write sz3.config
    config_lines = ['[GlobalSettings]']
    for k, v in options.items():
        if k == 'tuple':
            continue
        config_lines.append(f"{k} = {v}")
        if k == 'OpenMP':
            config_lines.append("[AlgoSettings]")
        
    config_text = "\n".join(config_lines)
    with open("sz3.config", "w") as f:
        f.write(config_text)
        
    # Run `./print_h5repack_args -c sz3.config`
    stream = os.popen('./print_h5repack_args -c sz3.config')
    output = stream.read()
    cd_tuple = output.split("=")[-1].strip().split(",")[3:]
    options['tuple'] = tuple(int(x) for x in cd_tuple)
    
    
def benchmark_zstd(tests, source_file, out_file, rerun=False):
    """
    Benchmark zstd compression on specified branches of the HDF5 file.
    'tests' is a dictionary containing lists of parameters to vary.
    tests['branches'] = [list of branches to test]
    tests['chunk_sizes'] = [list of chunk sizes to test]
    tests['clevels'] = [list of zstd compression levels to test]
    """
    
    zstd_id = 32015
    
    compressor_settings = {
        'compressor': 'zstd',
        'id': zstd_id,
        'options': {
            'clevel': 5
        }
    }
    
    for branch in tests['branches']:
        benchmark = CompressionBenchmark(source_file, out_file, [branch])
        for chunk_size in tests['chunk_sizes']:
            for clevel in tests['clevels']:
                print(f"Running zstd on {branch} with chunk size {chunk_size} and clevel {clevel}")
                settings = compressor_settings.copy()
                settings['chunk_size'] = chunk_size
                settings['options'] = settings['options'].copy()
                settings['options']['clevel'] = clevel
                settings['options']['tuple'] = (clevel,)
                settings['dst_name'] = f"clevel{clevel}_chunksize{chunk_size}"
                
                benchmark.compress(settings, rerun=rerun)
                
def benchmark_sz3(tests, source_file, out_file, rerun=False, probe=False):
    """
    Benchmark sz3 compression on specified branches of the HDF5 file.
    'tests' is a dictionary containing lists of parameters to vary.
    tests['branches'] = [list of branches to test]
    tests['chunk_sizes'] = [list of chunk sizes to test]
    tests['abs_error_bounds'] = [list of absolute error bounds to test]
    tests['algorithms'] = [list of prediction algorithms to test]
    """
    
    sz3_id = 32024
    
    compressor_settings = {
        'compressor': 'sz3',
        'id': sz3_id,
        'options': {
            'CmprAlgo': 'ALGO_LORENZO_REG',
            'ErrorBoundMode': 'ABS',
            'AbsErrorBound': '1',
            'RelErrorBound': '1e-3',
            'PSNRErrorBound': '70',
            'L2NormErrorBound': '.333',
            'OpenMP': 'NO',
            'InterpolationAlgo': 'INTERP_ALGO_CUBIC',
            'InterpolationDirection': '0',
            'Lorenzo': 'Yes',
            'Lorenzo2ndOrder': 'No',
            'Regression': 'Yes',
            'Regression2ndOrder': 'No',
            'QuantizationBinTotal': '65536',
            'tuple': ()
        }
    }
    
    for branch in tests['branches']:
        benchmark = CompressionBenchmark(source_file, out_file, [branch])        
        for chunk_size in tests['chunk_sizes']:
            # print(f"CHUNKSIZE: {chunk_size}")
            for abs_error_bound in tests['abs_error_bounds']:
                # print(f"ABSERRORBOUND: {abs_error_bound}")
                for algorithm in tests['algorithms']:
                    # print(f"ALGORITHM: {algorithm}")
                    print(f"Running sz3 on {branch} with chunk size {chunk_size}, abs error bound {abs_error_bound}, and algorithm {algorithm}")
                    settings = compressor_settings.copy()
                    settings['chunk_size'] = chunk_size
                    settings['options'] = settings['options'].copy()
                    settings['options']['AbsErrorBound'] = str(abs_error_bound)
                    settings['options']['CmprAlgo'] = algorithm
                    get_sz3_tuple(settings['options'])
                    print(f"sz3 tuple: {settings['options']['tuple']}")
                    settings['dst_name'] = f"abs{abs_error_bound}_alg{algorithm}_chunksize{chunk_size}"
                    
                    benchmark.compress(settings, rerun=rerun, probe=probe)
                    
def benchmark_zfp(tests, source_file, out_file, rerun=False):
    """
    Benchmark zfp compression on specified branches of the HDF5 file.
    'tests' is a dictionary containing lists of parameters to vary.
    tests['branches'] = [list of branches to test]
    tests['chunk_sizes'] = [list of chunk sizes to test]
    tests['precision'] = [list of precision values to test]
    """
    
    zfp_id = 32013
    
    compressor_settings = {
        'compressor': 'zfp',
        'id': zfp_id,
        'obj': hdf5plugin.Zfp,
        'options': {
            'precision': 16
        }
    }
    
    for branch in tests['branches']:
        benchmark = CompressionBenchmark(source_file, out_file, [branch])        
        for chunk_size in tests['chunk_sizes']:
            for precision in tests['precision']:
                print(f"Running zfp on {branch} with chunk size {chunk_size} and precision {precision}")
                settings = compressor_settings.copy()
                settings['chunk_size'] = chunk_size
                settings['options'] = settings['options'].copy()
                settings['options']['precision'] = precision
                settings['dst_name'] = f"prec{precision}_chunksize{chunk_size}"
                
                benchmark.compress(settings, raw_plugin=False, rerun=rerun)
                    
def main():
    """
    Main function to run compression benchmark.
    """    
    branches = [
        "AnalysisJetsAuxDyn/pt",
        "AnalysisJetsAuxDyn/eta",
        "AnalysisJetsAuxDyn/phi",
    ]
    
    chunk_sizes = [1024 * x for x in [32, 64, 128]]
    
    zstd_tests = {
        'branches': branches,
        'chunk_sizes': chunk_sizes,
        'clevels': [5],
    }
    
    sz3_pt_tests = {
        'branches': ['AnalysisJetsAuxDyn/pt'],
        'chunk_sizes': chunk_sizes,
        'abs_error_bounds': [10, 5],
        'algorithms': ['ALGO_INTERP_LORENZO', 'ALGO_INTERP', 'ALGO_LORENZO_REG']
    }
    
    sz3_eta_tests = {
        'branches': ['AnalysisJetsAuxDyn/eta'],
        'chunk_sizes': [1024 * x for x in [32, 64, 128]],
        'abs_error_bounds': [1, 1e-1, 1e-2, 1e-3],
        'algorithms': ['ALGO_INTERP_LORENZO', 'ALGO_INTERP', 'ALGO_LORENZO_REG']
    }
    
    sz3_phi_tests = {
        'branches': ['AnalysisJetsAuxDyn/phi'],
        'chunk_sizes': [1024 * x for x in [15, 32, 64, 128]],
        'abs_error_bounds': [1, 1e-1, 1e-2, 1e-3],
        'algorithms': ['ALGO_INTERP_LORENZO', 'ALGO_INTERP', 'ALGO_LORENZO_REG']
    }
    
    zfp_tests = {
        'branches': branches,
        'chunk_sizes': chunk_sizes,
        'precision': [8, 9, 10, 11, 12, 13, 14, 15, 16]
    }
    
    source_file = "/mnt/d/data/data.h5"
    out_file = "/mnt/d/data/compressed.h5"
    # out_file = "/mnt/d/data/dummy.h5"
    # out_file = "/mnt/d/data/probe.h5"
    
    # benchmark_zstd(zstd_tests, source_file, out_file, rerun=True)
    # benchmark_sz3(sz3_pt_tests, source_file, out_file, rerun=True)
    # benchmark_sz3(sz3_eta_tests, source_file, out_file, rerun=True)
    # benchmark_sz3(sz3_phi_tests, source_file, out_file, rerun=True)
    benchmark_zfp(zfp_tests, source_file, out_file, rerun=True)
    
if __name__ == "__main__":
    main()