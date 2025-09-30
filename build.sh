#!/bin/bash
set -e  # stop if any command fails

echo "=== Cleaning old build directory ==="
rm -rf build

echo "=== Running CMake (configure + generate) ==="
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo "=== Building project in Release mode ==="
cmake --build build --config Release

echo "=== Done! ==="
