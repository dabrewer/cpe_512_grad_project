#!/bin/bash
CFG_PATH=config
OUT_PATH=output
BIN_PATH=bin

# Ensure the output directories exist
mkdir -p $OUT_PATH
mkdir -p $OUT_PATH/post
mkdir -p $BIN_PATH

# Build executables
g++ -Isrc_serial/include/ src_serial/main.cpp src_serial/mesh.cpp src_serial/node.cpp -o bin/main_serial -fopenmp -O3
g++ -Isrc/include/ src/main.cpp src/mesh.cpp src/node.cpp -o bin/main -fopenmp -O3