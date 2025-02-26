#!/bin/bash

# Run test-zlib with every available distribution
for i in {3..4}
do
    # Run test-zlib with the current value of i
    ./test-zlib $i | tee test-zlib-$i.log
done