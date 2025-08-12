import pandas as pd

stats_3g = "3G-data-compression-stats.csv"
stats_13g = "13G-data-compression-stats.csv"
stats_134g = "134G-data-compression-stats.csv"

# Read and combine data stats
df_3g = pd.read_csv(stats_3g)
df_13g = pd.read_csv(stats_13g)
df_134g = pd.read_csv(stats_134g)

df_3g['dataset'] = '3G'
df_13g['dataset'] = '13G'
df_134g['dataset'] = '134G'

df_all = pd.concat([df_3g, df_13g, df_134g], ignore_index=True)

del df_3g, df_13g, df_134g

# Add branch prefix
df_all['branch_name_prefix'] = df_all['branch_name'].apply(lambda x: x.split('.')[0])

# Simplify branch types
branch_types = df_all['branch_type'].values
branch_type_map = {}

for branch_type in branch_types:
    if branch_type.startswith('DataVector<xAOD::'):
        branch_type_map[branch_type] = 'DataVector<xAOD::*>'
    elif branch_type.startswith('int'):
        branch_type_map[branch_type] = 'int'
    elif branch_type.startswith('std::vector<int'):
        branch_type_map[branch_type] = 'std::vector<int>'
    elif branch_type.startswith('std::vector<std::vector<ElementLink'):
        branch_type_map[branch_type] = 'std::vector<std::vector<ElementLink<*>>'
    elif branch_type.startswith('std::vector<std::vector<int'):
        branch_type_map[branch_type] = 'std::vector<std::vector<int*>>'
    elif branch_type.startswith('std::vector<uint'):
        branch_type_map[branch_type] = 'std::vector<uint*>'
    elif branch_type.startswith('uint'):
        branch_type_map[branch_type] = 'uint*'
    elif branch_type.startswith('vector<ElementLink<'):
        branch_type_map[branch_type] = 'vector<ElementLink<*>>'
    elif branch_type.startswith('vector<'):
        branch_type_map[branch_type] = 'vector<*>'
    elif branch_type.startswith('xAOD::'):
        branch_type_map[branch_type] = 'xAOD::*'
    else:
        branch_type_map[branch_type] = branch_type


df_all['branch_type_group'] = df_all['branch_type'].map(branch_type_map)

# Add column determining if branch is std::vector<float>
df_all['is_std_vector_float'] = df_all['branch_type'].str.startswith('std::vector<float')

# Rename bytes columns
bytes_map = {
    'compressed_bytes': 'bytes_compressed',
    'uncompressed_bytes': 'bytes_uncompressed'
}

df_all = df_all.rename(columns=bytes_map)

# Sort columns alphabetically
df_all = df_all.reindex(sorted(df_all.columns), axis=1)

# Write to single csv
df_all.to_csv("combined_compression_stats.csv", index=False)