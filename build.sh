#!/bin/bash
CFG_PATH=config
OUT_PATH=output
BIN_PATH=bin

# Ensure the output directories exist
mkdir -p $OUT_PATH
mkdir -p $BIN_PATH

# Build executable
g++ -Isrc/include/ src/main.cpp src/mesh.cpp src/node.cpp -o bin/main_serial
