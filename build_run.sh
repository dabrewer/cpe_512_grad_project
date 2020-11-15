#!/bin/bash
CFG_PATH=config
OUT_PATH=output


# Build executable
g++ -Isrc/include/ src/main.cpp src/geometry.cpp src/node.cpp -o bin/main

# Run executable
./bin/main $CFG_PATH/mesh.cfg $CFG_PATH/boundary.cfg $CFG_PATH/sor.cfg $OUT_PATH/mesh.out $OUT_PATH/stat.out

# Plot result
gnuplot <<- EOF
    set term x11 persist
    set pm3d map
    set view map
    splot "output/mesh.out" using 1:2:3 with image
    pause -1
EOF
