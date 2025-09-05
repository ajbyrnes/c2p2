import os
import pandas as pd

results_dir = "csv"

results_df = pd.DataFrame()

for file in os.listdir(results_dir):
    temp_df = pd.read_csv(os.path.join(results_dir, file))
    temp_df["resultsFile"] = file
    results_df = pd.concat([results_df, temp_df], ignore_index=True)
    
# Drop empty columns
results_df = results_df.dropna(axis=1, how='all')

# Add compressor name
results_df['compressorName'] = results_df['compressor'].str.split('{').str[0]

# Add input size in MB
results_df['inputSizeMB'] = results_df['inputSizeBytes'] / (1024 * 1024)


# Remap data name
data_name_map = {
    'AnalysisJetsAuxDyn.pt': 'pt',
    'AnalysisJetsAuxDyn.eta': 'eta',
    'AnalysisJetsAuxDyn.phi': 'phi',
}

results_df['dataName'] = results_df['dataName'].map(data_name_map).fillna(results_df['dataName'])

# Remap compression algorithm name
#  0 = ALGO_LORENZO_REG
#  1 = ALGO_INTERP_LORENZO
#  2 = ALGO_INTERP
#  3 = ALGO_NOPRED
#  4 = ALGO_LOSSLESS

algorithm_name_map = {
    0: "SZ3, Lorenzo Predictor (Regression)",
    1: "SZ3, Lorenzo Predictor (Interpolation)",
    2: "SZ3, Interpolation",
    3: "SZ3, No Predictor",
    4: "SZ3, Lossless Compression"
}

results_df['algorithmName'] = results_df['algorithm'].map(algorithm_name_map).fillna(results_df['algorithm'])

# If no algorithmName, use algorithm
results_df['algorithmName'] = results_df['algorithmName'].fillna('Bit Truncation')

# Add downgraded column
results_df['downgraded'] = results_df['maxAbsError'] == 0

# Add combination compressor-branch
# results_df['compressorBranch'] = results_df['compressorName'] + '-' + results_df['dataName']

# Remove .csv from end of result file
results_df['resultsFile'] = results_df['resultsFile'].str.replace('.csv', '', regex=False)

# Organize columns
columns_map = {
    'absBound': 'param_absBound',
    'algorithm': 'param_algorithm',
    'algorithmName': 'param_algorithmName',
    'compressionLevel': 'param_compressionLevel',
    'compressor' : 'param_compressor',
    'compressorName': 'param_compressorName',
    'dataFile': 'param_inputFile',
    'inputSizeBytes': 'param_inputSizeBytes',
    'inputSizeMB': 'param_inputSizeMB',
    'dataName': 'param_variable',
    'mantissaBits': 'param_mantissaBits',
    'relBound': 'param_relBound',
    'host': 'meta_host',
    'resultsFile': 'meta_outputFile',
    'timestamp': 'meta_timestamp',
    'compressionRatio': 'result_compressionRatio',
    'compressionTimeMs': 'result_timeToCompressMs',
    'decompressionTimeMs': 'result_timeToDecompressMs',
    'downgraded': 'result_downgraded',
    'maxAbsError': 'result_absErrorMax',
    'meanAbsError': 'result_absErrorMean',
    'medianAbsError': 'result_absErrorMedian',
    'minAbsError': 'result_absErrorMin',
    'q1AbsError': 'result_absErrorQ1',
    'q3AbsError': 'result_absErrorQ3',
    'stdDevAbsError': 'result_absErrorStdDev',
    'maxRelErrorPct': 'result_relErrorPctMax',
    'meanRelErrorPct': 'result_relErrorPctMean',
    'medianRelErrorPct': 'result_relErrorPctMedian',
    'minRelErrorPct': 'result_relErrorPctMin',
    'q1RelErrorPct': 'result_relErrorPctQ1',
    'q3RelErrorPct': 'result_relErrorPctQ3',
    'stdDevRelErrorPct': 'result_relErrorPctStdDev',
    'outputSizeBytes': 'result_outputSizeBytes'
}

results_df = results_df.rename(columns=columns_map)

# Group files by input size
results_df['inputSizeGroup'] = pd.cut(
    results_df['param_inputSizeMB'],
    bins=[0, 1, 3, 4, 5, 6, 7, 11],
    labels=['0-1MB', '1-3MB', '3-4MB', '4-5MB', '5-6MB', '6-7MB', '10+MB'],
    right=False
)

# Sort columns alphabetically
results_df = results_df.reindex(sorted(results_df.columns), axis=1)

# # Add lossless stats
# stats_df = pd.read_csv("combined_compression_stats.csv")

# # Select branches
# stats_df = stats_df[
#     (stats_df['branch_name'] == 'AnalysisJetsAuxDyn.pt') |
#     (stats_df['branch_name'] == 'AnalysisJetsAuxDyn.eta') |
#     (stats_df['branch_name'] == 'AnalysisJetsAuxDyn.phi')
# ]

# stats_df['param_variable'] = stats_df['branch_name'].str.split('.').str[-1]

# # Rename filename and compression_ratio columns
# stats_df = stats_df.rename(columns={
#     'filename': 'param_inputFile',
#     'compression_ratio': 'stats_losslessCompression'
# })

# # Join stats_df to results_df
# results_df = results_df.merge(stats_df[['param_inputFile', 'param_variable', 'stats_losslessCompression']], on=['param_inputFile', 'param_variable'], how='left')

results_df.to_csv("benchmark_results.csv", index=False)