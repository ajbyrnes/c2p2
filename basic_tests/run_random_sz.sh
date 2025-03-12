#!/bin/bash

exec=random_sz
trials=10
datasize=25

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
    for eb in {0..5}
    do
        for alg in {0..4}
        do
            for ialg in {0..1}
            do
                ./$exec $eb $alg $ialg $datasize $dist $trials | tee $exec-d=$dist-eb=$eb-alg=$alg-ialg=$ialg.log
            done
        done
    done
done


# Remove copied files
rm $exec