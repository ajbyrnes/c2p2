# Uses ATLAS Open Magic to download a specific dataset
# Usage: python download_atlas_data.py <dest>
# Example: python download_atlas_data.py /path/to/destination

import os
import re
import sys

import argparse
import atlasopenmagic as atom

parser = argparse.ArgumentParser(description='Download ATLAS Open Data')

# Add argument for target dataset
# Data from one run = Set of files with the same prefix i.e. DAOD_PHYSLITE.37019932.*
# The `target` argument is the number in that prefix i.e. DAOD_PHYSLITE.<target>.*
# You can browse runs in the ATLAS Open Data portal
# https://opendata.cern.ch/record/80001
parser.add_argument('target', type=str, help='Target dataset to download (e.g., DAOD_PHYSLITE.37019932)')

# Add argument for destination directory
parser.add_argument('dest', type=str, help='Destination directory to save the data')

args = parser.parse_args()

# The default current release in atom is '2024r-pp',
# "2024 Open Data for research release for proton-proton collision"
# This is exactly what we want so nothing needs to be done to set the release unless
#       a) We actually want a different data release
#       b) The default release changes in the future
current_release = atom.get_current_release()

# Search current release for items in the target dataset
target = f'DAOD_PHYSLITE.{args.target}'
print(f'Searching release {current_release} for {target}')

target_re = re.compile(rf'{target}\.')
urls = atom.get_urls('data', protocol='https')
print(urls[0])

target_urls = [url for url in urls if target_re.search(url)]

if not target_urls:
    print(f"No URLs found for target: {target}")
    sys.exit(3)

print(f'Found {len(target_urls)} URLs for {target}')
for url in target_urls[:5]:
    print("\t", url)
print(f'\t... and {len(target_urls) - 5} more')

# Extract the directory and filename from the first URL
url = target_urls[0]
pattern = r'/([^/]+)/(DAOD_PHYSLITE\.\d+)'
match = re.search(pattern, url)
dirname = match.group(1)
dataname = match.group(2)

# Prompt for download
input(f"Download {len(target_urls)} files to {args.dest}/{dirname}/{dataname}? Press Enter to continue...")

# Download to directory
for url in target_urls:
    # Extract filename
    filename = url.split('/')[-1]
    
    # Skip file if it already exists in directory
    if os.path.exists(f'{args.dest}/{dirname}/{dataname}/{filename}'):
        print(f"File already exists: {filename}")
        continue
    
    # Download the file
    os.system(f'echo {args.dest}/{dirname}/{dataname}')
    os.system(f'wget -P {args.dest}/{dirname}/{dataname} {url}')
