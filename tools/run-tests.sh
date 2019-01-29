#!/bin/bash

EMPTY_STRING=""

# Check config file
if [[ $(find . -name "CTestTestfile.cmake" | wc -l) == 0 ]]; then
	echo "[ERROR] Fail to find CTestTestfile.cmake"
fi

# Get CPU cores count
CPU_CORES=$(nproc)

# Get config directory
configPath=$(find . -name "CTestTestfile.cmake" | head -n 1)
configDir="${configPath/CTestTestfile.cmake/$EMPTY_STRING}"

# Go to config directory
cd "$configDir"

# Run tests with {CPU_CORES} workers
ctest -C debug -VV -j "$CPU_CORES"
