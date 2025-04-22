#!/bin/bash

OUTPUT_DIR="build"
TARGET_NAME="2d-shooter"

build() {
    if [ ! -d $OUTPUT_DIR ]; then
        mkdir $OUTPUT_DIR
    fi
    cd $OUTPUT_DIR || exit 1
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    cmake --build . --config Debug
}

release() {
    if [ ! -d $OUTPUT_DIR ]; then
        mkdir $OUTPUT_DIR
    fi
    cd $OUTPUT_DIR || exit 1
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build . --config Release
}

host() {
    build
    ./debug/$TARGET_NAME host
}

client() {
    build
    ./debug/$TARGET_NAME client
}

if [ "$1" == "host" ]; then
    host

elif [ "$1" == "client" ]; then
    client

elif [ "$1" == "clean" ]; then
    rm -rf $OUTPUT_DIR
    echo "Cleaned build directory"

elif [ "$1" == "release" ]; then
    release
    echo "Release build completed"

else
    build
    echo "Debug build completed"
fi

