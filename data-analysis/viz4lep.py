import uproot

import numpy as np
import awkward as ak

import pandas as pd

import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots

datadir = '/home/abyrnes/data/atlas/'
dataA = datadir + '4lep/data_A.4lep.root'
dataB = datadir + '4lep/data_B.4lep.root'
dataC = datadir + '4lep/data_C.4lep.root'
dataD = datadir + '4lep/data_D.4lep.root'

