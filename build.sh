#!/usr/bin/zsh

mkdir -p build
cd build
cmake .. -Wno-dev
make
