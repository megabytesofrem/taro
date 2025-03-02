#!/bin/bash

BUILD_COMMAND="cd build && cmake .. && make"

# Create build directory if it doesnt exist
if [ ! -d "build" ]; then
    echo "Creating build directory (initially)"
    mkdir -p build
    cd build && cmake ..
fi

[ -d "build" ] && eval $BUILD_COMMAND
[ -f "taro"  ] && ./taro
