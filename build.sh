#!/bin/bash

CORE_COUNT=$(nproc)

echo "[INFO] Use $CORE_COUNT CPU-Cores for make"

mkdir -p build
cd build

cmake ..
make -j$CORE_COUNT iso
