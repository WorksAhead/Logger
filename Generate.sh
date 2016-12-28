#!/bin/sh

rm -rf ./cos-cpp-sdk-v4
git clone https://github.com/tencentyun/cos-cpp-sdk-v4.git

rm -rf ./Build
mkdir -p ./Build/Bin
cp ./cosconfig.json ./Build/Bin

cd Build
cmake -DCMAKE_BUILD_TYPE=Release ..
