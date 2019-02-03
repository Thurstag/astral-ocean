#!/bin/bash

# Create build directory
mkdir build

# Configure
cd build
cmake ../

# Build
cmake --build .

# Run tests
ctest -C debug -VV -j 4
