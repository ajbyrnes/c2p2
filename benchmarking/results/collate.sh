#!/usr/bin/bash

# Outputs concatenation of all log files, plus a header/footer

for filename in ./*.log
do
    echo $filename
    cat $filename
    echo "=========="
done