import numpy as np
import pandas as pd

import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots 


results_df = pd.read_csv("benchmark_results.csv")

def select_data(variable):
    # Drop downgraded rows
    plot_df = results_df[(results_df['result_downgraded'] == False) | (results_df['param_compressorName'] == 'TruncCompressor')]
    plot_df = plot_df[plot_df['result_relErrorPctMean'] < 0.1]
    plot_df = plot_df[plot_df['param_variable'] == variable]

    plot_df = plot_df.groupby(['param_compressorName', 'param_algorithmName', 'inputSizeGroup']).median(numeric_only=True).reset_index()

    return plot_df.sort_values(by='param_inputSizeBytes')

def plot_data(plot_df):
    colors = [
        "#0b80c3",  # ATLAS blue
        "#c34e0b",  # Complementary
        "#c30b80",  # Triadic 1
        "#80c30b",  # Triadic 2
        "#0b24c3",  # Analogous 1
        "#0bc3aa",  # Analogous 2
        "#c30b24",  # Split 1
        "#c3aa0b",  # Split 2
        "#09669c",  # Monochromatic 1
        "#0d9aea"   # Monochromatic 2
    ]

    algorithm_color_map = {
        'Bit Truncation': colors[0],
        'SZ3, No Predictor': colors[1],
        'SZ3, Lorenzo Predictor (Regression)': colors[2],
        'SZ3, Lorenzo Predictor (Interpolation)' : colors[3],
        'SZ3, Interpolation': colors[5],
    }

    fig = px.scatter(
        plot_df,
        x = 'inputSizeGroup',
        y = 'result_compressionRatio',
        color = 'param_algorithmName',
        # symbol = 'param_absBound',
        color_discrete_map=algorithm_color_map,
        category_orders = {
            'inputSizeGroup': ['0-1MB', '1-3MB', '3-4MB', '4-5MB', '5-6MB', '6-7MB', '10+MB'],
        }
    )
    
    fig.add_trace(
        go.Scatter(
            x = ['0-1MB', '1-3MB', '3-4MB', '4-5MB', '5-6MB', '6-7MB', '10+MB'],
            y = plot_df.groupby('inputSizeGroup').mean(numeric_only=True)['stats_losslessCompression'],
            mode = 'lines',
            line = dict(color='gray', dash='dash'),
            name = 'Lossless Compression (ZSTD, Level 5)'
        )
    )
    

    fig.update_traces(
        marker = dict(
            size = 10,
            line = dict(width=2)
        ),
        mode = 'markers+lines'
    )

    fig.update_layout(
        title = dict(
            text = f'Compression Ratio vs Input Size for Jet {variable}'
        ),
        xaxis = dict(
            title = 'Input Size'
        ),
        yaxis = dict(
            title = 'Compression Ratio'
        ),
        legend = dict(
            title = 'Compressor/Algorithm',
        ),
        height = 500,
        width = 1000
    )

    return fig
    
for variable in ['pt', 'eta', 'phi']:
    fig = plot_data(select_data(variable))
    # fig.show()
    fig.write_image(f"compression_ratio_vs_input_size_{variable}.svg")