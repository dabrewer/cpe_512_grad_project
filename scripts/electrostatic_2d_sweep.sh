#!/bin/bash

####################################################################################################
# DEFINITIONS
####################################################################################################
# Path definitions
CFG_PATH=config
OUT_PATH=output
BIN_PATH=bin

# Run characteristics
THREAD_RANGE="1 2 4 8 16 32"
SIZE_RANGE="2.0 1.5 1.25 1.0 0.75 0.65 0.55 0.5 0.4 0.3 0.25"

####################################################################################################
# PROGRAM EXECUTION FUNCTIONS
####################################################################################################
# Function to run the serial configuration 100 times for each size configuration
run_serial()
{
    for i in {1..100}
    do
        for s in $SIZE_RANGE
        do
            ./bin/main_serial $CFG_PATH/mesh_${s}.cfg $CFG_PATH/boundary.cfg $CFG_PATH/sor.cfg $OUT_PATH/mesh_serial_${s}.out $OUT_PATH/stat.out
        done
    done
}

# Function to run each OMP thread/size configuration 100 times
run_omp()
{
    for i in {1..100}
    do
        for t in $THREAD_RANGE
        do
            for s in $SIZE_RANGE
            do
                ./bin/main $t $CFG_PATH/mesh_${s}.cfg $CFG_PATH/boundary.cfg $CFG_PATH/sor.cfg $OUT_PATH/mesh_${t}_${s}.out $OUT_PATH/stat.out
            done
        done
    done
}

####################################################################################################
# MAIN ROUTINE
####################################################################################################
run_serial
run_omp