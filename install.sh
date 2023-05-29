#!/bin/bash
if [ ! -e build ]; then
    mkdir build
fi
cd build

cmake .. -GNinja
ninja
sudo ninja install
