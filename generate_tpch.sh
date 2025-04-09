#!/bin/bash
#
# generate_tpch.sh
#
# This script changes into the tpch-dbgen directory, builds the project using make,
# and then runs dbgen to generate the TPC‑H data files.
# The output table files (.tbl) are then moved to a specified output folder.
#
# Usage: ./generate_tpch.sh [scale_factor] [output_directory]
#   - scale_factor: The TPC‑H scale factor (default is 1)
#   - output_directory: Directory where the generated .tbl files will be stored (default is "tpch_data")
#
# Assumptions:
#   - The tpch-dbgen repository is located in a subdirectory named "tpch-dbgen" relative to this script.
#   - The Makefile in the repository will build the "dbgen" executable.
#

# Set default parameters
SCALE_FACTOR=${1:-1}
OUTPUT_DIR=${2:-tpch_data}
TPCH_DIR="tpch-dbgen"

# Check if the tpch-dbgen directory exists
if [ ! -d "$TPCH_DIR" ]; then
    echo "Error: Directory '$TPCH_DIR' does not exist."
    echo "Please clone the tpch-dbgen repository into the current directory:"
    echo "  git clone https://github.com/electrum/tpch-dbgen.git"
    exit 1
fi

echo "Changing directory to $TPCH_DIR..."
cd "$TPCH_DIR" || exit 1

# Build the project using make
echo "Running 'make clean' and 'make'..."
make clean
make
if [ $? -ne 0 ]; then
    echo "Error: Build failed. Please check the make output for errors."
    exit 1
fi

# Verify that the dbgen executable exists and is executable
if [ ! -x "./dbgen" ]; then
    echo "Error: dbgen executable not found after build."
    exit 1
fi

echo "Build successful."
echo "Running dbgen with scale factor $SCALE_FACTOR..."

# Remove any existing .tbl files to avoid mixing old data
rm -f *.tbl

# Generate the TPC‑H tables (files like lineitem.tbl, orders.tbl, etc.)
./dbgen -s "$SCALE_FACTOR"
if [ $? -ne 0 ]; then
    echo "Error: dbgen encountered an error during data generation."
    exit 1
fi

# Return to the parent directory so that output directory can be created there
cd ..

# Create the output directory if it does not already exist
mkdir -p "$OUTPUT_DIR"

# Move the generated .tbl files from the tpch-dbgen folder to the output directory
mv "$TPCH_DIR"/*.tbl "$OUTPUT_DIR"/

echo "TPC‑H data generation complete."
echo "Generated table files have been moved to the output folder: $OUTPUT_DIR"
ls -1 "$OUTPUT_DIR"
