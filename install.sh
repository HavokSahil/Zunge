#!/bin/bash

sudo apt-get update
sudo apt-get install -y gcc-11 g++-11 libzip-dev libxml2-dev cmake libmp3lame-dev python3 python3-pip lame festival libavformat-dev libavcodec-dev libavutil-dev python3-setuptools python3-wheel libncurses-dev zlib1g-dev
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100

cmake -S . -B build
cd build
make
sudo make install
make clean
cd ..

echo "Installation complete."
