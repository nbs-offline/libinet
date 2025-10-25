#!/bin/bash
set -e

unset CPATH
unset C_INCLUDE_PATH
unset CPLUS_INCLUDE_PATH
unset LIBRARY_PATH

mkdir -p build
cd build

make -j$(nproc) VERBOSE=1

echo "Build completed successfully."