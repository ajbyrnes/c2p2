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

if len(sys.argv) > 1:
    target = sys.argv[1]
    print(f"Using target: {target}")
else:
    target = 'DAOD_PHYSLITE.37019878'
    print("No target specified. Using default: DAOD_PHYSLITE.37019878")
    
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
filename = match.group(2)

print(f'Directory: {dirname}')
print(f'Filename: {filename}') 

# Prompt for download
input(f"Download {len(target_urls)} files to /mnt/d/{dirname}/{filename}? Press Enter to continue...")

# Download to directory
for url in target_urls:
    # Skip file if it already exists in directory
    if os.path.exists(f'/mnt/d/{dirname}/{filename}'):
        print(f"File already exists: {filename}")
        continue
    
    # Download the file
    os.system(f'echo /mnt/d/{dirname}/{filename}')
    os.system(f'wget -P /mnt/d/{dirname}/{filename} {url}')