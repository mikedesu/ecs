#!/usr/bin/zsh

mkdir -p build
cd build
cmake .. -Wno-dev
#CMAKE_EXPORT_COMPILE_COMMANDS=1 cmake .. -Wno-dev
make
