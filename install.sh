#/bin/bash

# SCRIPT TO COPY HEADERS INTO INCLUDE FOLDER

# Define variables
SRC_RELATIVE_PATH=src/main/
INCLUDE_RELATIVE_PATH=include/
INCLUDE_PATH_FROM_SRC=../../include

# Check include folder
if [[ -d $INCLUDE_RELATIVE_PATH ]]; then
	echo "[INFO] Flush include directory"
	rm -dRf "$INCLUDE_RELATIVE_PATH/ao"
fi

# Create include folder
echo "[INFO] Create include folder"
mkdir -p "$INCLUDE_RELATIVE_PATH/ao"

# Go to source folder
cd $SRC_RELATIVE_PATH

# List files
files=($(find core/ vulkan/ -iregex ".*\.\(h\|hpp\)"))

# Loop over files
echo "[INFO] Copy files into $INCLUDE_RELATIVE_PATH"
for file in ${files[@]}
do
    	cp --parents $file "$INCLUDE_PATH_FROM_SRC/ao/"
done
