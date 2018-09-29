#!/bin/bash

cmake  -DCMAKE_BUILD_TYPE=Debug ./ -Bbuild
cd ./build
make
