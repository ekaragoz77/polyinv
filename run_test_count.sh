#!/bin/bash

# List of Inverse Methods
INVERSE_METHODS=("BYI" "FLT" "CEA" "TYT" "SAC")

EXT_DEGS=("10499" "12323" "24659" "24781" "27067" "27581" "40973")

FUNCS=("gf2x_mod_mul" "gf2x_mod_sqr" "mul64" "sqr64") 

# Clean the previous executables
echo "Cleaning the previous executables (test_cnt_P*)..."
rm -f test_cnt_P* 

for INVERSE_METHOD in "${INVERSE_METHODS[@]}"
do
    for EXT_DEG in "${EXT_DEGS[@]}"
    do        
        echo "INVERSE_METHOD=${INVERSE_METHOD}, EXT_DEG=${EXT_DEG}"
        make test_count INVERSE_METHOD=${INVERSE_METHOD} EXT_DEG=${EXT_DEG}

        for FUNC in "${FUNCS[@]}"
        do
            COUNT=$(./test_cnt_P${EXT_DEG}_${INVERSE_METHOD} | grep -c "${FUNC}")
            echo "  ${FUNC} : ${COUNT}"
        done
    done
done