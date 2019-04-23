#!/bin/bash

# Create build directory
mkdir build

# Configure
cd build
cmake ../

# Build
cmake --build . --config Release

# Run tests
ctest -C Release -VV -j 4
