#!/bin/bash

# Create build directory
mkdir build

# Configure
cd build
cmake ../

# Build
cmake --build .

# TODO: ADD TESTS
