#!/bin/bash
set -e
echo "Cleaning up..."
rm -rf build && rm -rf bin
printf "\n"
echo "Building..."
mkdir build && cd build && cmake .. && make  #&& make install 安装到指定目录，暂时不用
cd ..
echo "Build successful!"
printf "\n"
# echo "Running Alex..."
# printf "\n"
# ./bin/Alex
