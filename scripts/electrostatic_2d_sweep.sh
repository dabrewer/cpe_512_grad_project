#!/bin/bash

ITERATION_RANGE={1..10}
THREAD_RANGE="1 2 4 8 16 32"
SIZE_RANGE="2.0 1.5 1.0 0.75 0.5 0.25"

# Run the serial configuration 10 times for each size configuration
for i in $ITERATION_RANGE
do
    for s in $SIZE_RANGE
    do
        ./bin/main_serial $CFG_PATH/mesh_${s}.cfg $CFG_PATH/boundary.cfg $CFG_PATH/sor.cfg $OUT_PATH/mesh_${s}.out $OUT_PATH/stat_serial_${s}.out
    done
done

# Run each OMP thread/size configuration 10 times
for i in $ITERATION_RANGE
do
    for t in $THREAD_RANGE
    do
        for s in $SIZE_RANGE
        do
            ./bin/main $t $CFG_PATH/mesh_${s}.cfg $CFG_PATH/boundary.cfg $CFG_PATH/sor.cfg $OUT_PATH/mesh_${s}.out $OUT_PATH/stat_${t}_${s}.out
        done
    done
done

# TODO: calculate average of $OUT_PATH/stat_${t}_${s}.out
