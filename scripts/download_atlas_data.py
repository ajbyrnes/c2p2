#! /home/ajbyrnes/miniforge3/envs/env/bin/python

import os
import re
import sys

import atlasopenmagic as atom

# First argument is target
# Example: DAOD_PHYSLITE.37019878
# This is the dataset we want to download
# You can find the dataset in the ATLAS Open Data portal
# https://opendata.atlas.cern/data/

if len(sys.argv) != 2:
    print("Usage: python download_atlas_data.py <dest>")
    sys.exit(1)
    
dest = sys.argv[1]

target = 'DAOD_PHYSLITE.37019878'
print(f'Searching for URLs for target: {target}')
    
urls = atom.get_urls('data', protocol='https')
target_urls = [url for url in urls if target in url]

if not target_urls:
    print(f"No URLs found for target: {target}")
    sys.exit(1)

print(f'Found {len(target_urls)} URLs for {target}')
print('First 5 URLs:')
for url in target_urls[:5]:
    print("\t", url)

# Extract the filename from the URL
url = target_urls[0]
pattern = r'/([^/]+)/(DAOD_PHYSLITE\.\d+)'
match = re.search(pattern, url)

dirname = match.group(1)
dataname = match.group(2)

print(f'Directory: {dirname}')
print(f'Filename: {dataname}') 

# Prompt for download
input(f"Download {len(target_urls)} files to {dest}/{dirname}/{dataname}? Press Enter to continue...")

# Download to directory
for url in target_urls:
    # Extract filename
    filename = url.split('/')[-1]
    
    # Skip file if it already exists in directory
    if os.path.exists(f'{dest}/{dirname}/{dataname}/{filename}'):
        print(f"File already exists: {filename}")
        continue
    
    # Download the file
    os.system(f'echo {dest}/{dirname}/{dataname}')
    os.system(f'wget -P {dest}/{dirname}/{dataname} {url}')
    