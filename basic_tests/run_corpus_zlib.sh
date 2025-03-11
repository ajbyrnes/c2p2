#!/bin/bash

exec=corpus_zlib
trials=10

# Create new, unique directory to avoid overwriting existing logs
timestamp=$(date +%Y%m%d%H%M%S)
dir=$exec-$timestamp
mkdir $dir

# Copy executable and data files to new directory
cp $exec $dir/
cp -r data $dir/
cd $dir

# Run test-zlib with every available distribution
for level in {0..9}
do
    # Run test-zlib with the current value of i
    ./$executable $level $trials | tee $exec-l=$level.log
done

# Remove copied files
rm -rf data
rm $exec