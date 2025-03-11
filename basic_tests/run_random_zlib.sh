#!/bin/bash

exec=random_zlib
trials=10
datasize=100

# Create new, unique directory to avoid overwriting existing logs
timestamp=$(date +%Y%m%d%H%M%S)
dir=$exec-$timestamp
mkdir $dir

# Copy executable and data files to new directory
cp $exec $dir/
cd $dir


# Run test-zlib with every available distribution
for dist in {1..4}
do
    for level in {0..9}
    do
        # Run test-zlib with the current value of i
        ./$exec $level $datasize $dist $trials | tee $exec-d=$dist-s=$datasize-l=$level.log
    done
done


# Remove copied files
rm $exec