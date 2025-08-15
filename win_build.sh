#!/bin/env bash

mkdir win_build
cd win_build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./zig-win.cmake
cmake --build . -j$(nproc)
cd ..

