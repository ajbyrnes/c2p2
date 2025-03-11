#!/bin/bash

trials=1

# Run test-zlib with every available distribution
for dist in {1..4}
do
    for level in {0..9}
    do
        # Run test-zlib with the current value of i
        ./test-zlib $dist $level $trials | tee test-zlib-d=$dist-l=$level.log
    done
done