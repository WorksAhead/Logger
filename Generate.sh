#!/bin/sh

git clone https://github.com/tencentyun/cos-cpp-sdk-v4.git

rm -rf ./Build
mkdir -p ./Build/Bin
cp ./cosconfig.json ./Build/Bin
cd Build
cmake -DBOOST_ROOT=/usr/local/bin ..
