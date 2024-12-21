#!/bin/bash

# List of Extension Degrees
EXT_DEGS=("10499" "12323" "24659" "24781" "27067" "27581" "40973")

# Clean the previous executables
echo "Cleaning the previous executables (test_inv_P*)..."
rm -f test_inv_P* 

for EXT_DEG in "${EXT_DEGS[@]}"
do    
    echo "Running make test_inv with EXT_DEG=${EXT_DEG} for BYI"
    make test_inv EXT_DEG=${EXT_DEG} INVERSE_METHOD=BYI

    echo "Running make test_inv with EXT_DEG=${EXT_DEG} for others"
    make test_inv EXT_DEG=${EXT_DEG}
done
