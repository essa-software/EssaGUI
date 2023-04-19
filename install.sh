#!/bin/bash
if [ ! -e build ]; then
    mkdir build
fi
cd build

cmake .. -GNinja -DESSAGUI_EXAMPLES=0
ninja
sudo ninja install
