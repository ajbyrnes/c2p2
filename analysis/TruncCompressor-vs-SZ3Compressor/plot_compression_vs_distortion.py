import numpy as np
import pandas as pd

import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots 


results_df = pd.read_csv("benchmark_results.csv")

def select_data(variable, error_metric, threshold):
    # Drop downgraded rows
    plot_df = results_df[(results_df['result_downgraded'] == False) | (results_df['param_compressorName'] == 'TruncCompressor')]
    plot_df = plot_df[((results_df['param_compressorName'] == 'SZ3Compressor') & (results_df['param_algorithmName'] == 'SZ3, No Predictor')) | (results_df['param_compressorName'] == 'TruncCompressor')]
    plot_df = plot_df[plot_df[error_metric] <= threshold]
    plot_df = plot_df[plot_df['param_variable'] == variable]

    plot_df = plot_df.groupby(['param_compressor', 'param_algorithmName']).mean(numeric_only=True).reset_index()

    return plot_df.sort_values(by=['param_algorithmName', error_metric])

def plot_data(plot_df, error_metric, xmin, xmax):
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
        x = error_metric,
        y = 'result_compressionRatio',
        color = 'param_algorithmName',
        # symbol = 'param_absBound',
        color_discrete_map=algorithm_color_map,
        # facet_row = 'inputSizeGroup'
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
            text = f'Compression Ratio vs Data Distortion for Jet {variable}'
        ),
        xaxis = dict(
            title = 'Mean Pointwise Relative Error (%)',
            # range = [xmin, xmax]
        ),
        yaxis = dict(
            title = 'Compression Ratio'
        ),
        legend = dict(
            title = 'Compressor/Algorithm',
        ),
        height = 500,
        width = 1000,
    )

    return fig

    
plot_params = [
    ['pt', 0.005, -0.01, 0.1,],
    ['eta', 1, -0.01, 0.2],
    ['phi', 1, -0.001, 0.1],
]

for variable, threshold, xmin, xmax in plot_params:
    error_metric = 'result_relErrorPctMean'
    plot_df = select_data(variable, error_metric, threshold)
    fig = plot_data(plot_df, error_metric, xmin, xmax)

    fig.write_image(f"compression_ratio_vs_distortion_{variable}.svg")