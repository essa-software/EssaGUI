#!/bin/bash
if [ ! -e build ]; then
    mkdir build
fi
cd build

rm -rf util
git clone https://github.com/essa-software/util
cd util
./install.sh || exit 1
cd ..

rm -rf llgl
git clone https://github.com/sppmacd/llgl
cd llgl
./install.sh || exit 1
cd ..

cmake .. -GNinja
ninja
sudo ninja install
