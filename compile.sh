#!/bin/bash
mkdir -p cmake-build-debug
cd "./cmake-build-debug/"
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
cmake --build .
