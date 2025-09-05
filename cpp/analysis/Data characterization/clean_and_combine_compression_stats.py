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

# Read and combine data stats
df_3g = pd.read_csv(stats_3g)
df_13g = pd.read_csv(stats_13g)
df_134g = pd.read_csv(stats_134g)

df_3g['dataset'] = '3G'
df_13g['dataset'] = '13G'
df_134g['dataset'] = '134G'

df_all = pd.concat([df_3g, df_13g, df_134g], ignore_index=True)

del df_3g, df_13g, df_134g

# Add column with container
# If branch_name contains container, set container column to container name
containers = [
    'AnalysisElectrons', 'AnalysisJets', 'AnalysisLargeRJets',
    'AnalysisMuons', 'AnalysisPhotons', 'AnalysisSiHitElectrons',
    'AnalysisTauJets', 'AnalysisTrigMatch', 
    'BTagging_AntiKt4EMPFlow','BTagging_AntiKtVR30', 
    'CombinedMuonTrackParticles', 'ExtrapolatedMuonTrackParticles',
    'EventInfo', 'FourLeptonVertices', 'PrimaryVertices',
    'GSFConversionVertices', 'GSFTrackParticles', 
    'InDetTrackParticles', 'MuonSpectrometerTrackParticles',
    'Kt4EMPFlowEventShape', 
    'METAssoc_AnalysisMET', 'MET_Core_AnalysisMET', 
    'TauTracks', 'egammaClusters', 'index_ref', 'xTrigDecision',
]

df_all['container'] = 'other'

for container in containers:
    df_all.loc[df_all['branch_name'].str.contains(container), 'container'] = container

# Split branch names
df_all['branch_name_short'] = 'placeholder'
df_all.loc[df_all['branch_name_short'] == 'placeholder', 'branch_name_short'] = df_all['branch_name'].str.split('.').str[-1]
df_all.loc[df_all['branch_name_short'] == '', 'branch_name_short'] = df_all['branch_name']

# Simplify branch types
branch_types = df_all['branch_type'].values
branch_type_map = {}

for branch_type in branch_types:
    if 'xAOD' in branch_type:
        branch_type_map[branch_type] = 'xAOD'
    elif 'uint' in branch_type:
        branch_type_map[branch_type] = 'uint'
    elif 'int' in branch_type:
        branch_type_map[branch_type] = 'int'
    elif 'float' in branch_type:
        branch_type_map[branch_type] = 'float'
    elif 'double' in branch_type:
        branch_type_map[branch_type] = 'double'
    else:
        branch_type_map[branch_type] = 'other'

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