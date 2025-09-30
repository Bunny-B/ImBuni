@echo off
echo === Cleaning old build directory ===
if exist build (
    rmdir /s /q build
)

echo === Running CMake (configure + generate) ===
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo === Building project in Release mode ===
cmake --build build --config Release

echo === Done! ===
pause
