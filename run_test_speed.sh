#!/bin/bash

EXT_DEGS=("10499" "12323" "24659" "24781" "27067" "27581" "40973")

# Clean the previous executables
echo "Cleaning the previous executables (test_speed_P*)..."
rm -f test_speed_P* 

for EXT_DEG in "${EXT_DEGS[@]}"
do
    echo "Running make test_speed with EXT_DEG=${EXT_DEG} for BYI"
    make test_speed EXT_DEG=${EXT_DEG} INVERSE_METHOD=BYI

    echo "Running make test_speed with EXT_DEG=${EXT_DEG} for others"
    make test_speed EXT_DEG=${EXT_DEG}
done
