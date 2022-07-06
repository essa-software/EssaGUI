#!/bin/bash
if [ ! -e build ]; then
    mkdir build
fi
cd build

rm -rf util
git clone https://github.com/essa-software/util
cd util
CMAKE_OPTIONS=-DESSAUTIL_ENABLE_SFML=1 ./install.sh
cd ..

cmake .. -GNinja
ninja
sudo ninja install
