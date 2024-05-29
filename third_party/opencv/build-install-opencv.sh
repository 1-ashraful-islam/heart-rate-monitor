#!/bin/sh

# Download and unpack source
wget -O opencv.zip https://github.com/opencv/opencv/archive/refs/tags/4.9.0.zip
unzip opencv.zip

# create build directory
mkdir -p build install

# change into the build directory
cd build

# configure
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=../install ../opencv-4.9.0

# Build
cmake --build . --config Release -- -j8

# Install
make install

# (Optional) uncomment to remove source files
# cd ..
# rm -rf opencv.zip opencv-4.9.0
