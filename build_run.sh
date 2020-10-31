#!/bin/bash

# Build executable
g++ -Isrc/include/ src/main.cpp src/geometry.cpp src/node.cpp -o bin/main

# Run executable
./bin/main

# Plot result
gnuplot <<- EOF
    set term x11 persist
    set pm3d map
    set view map
    splot "output/mesh.out" using 1:2:3 with image
    pause -1
EOF
