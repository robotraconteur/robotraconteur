#!/bin/bash

set -e

if [ ! -f "RobotRaconteurCore/CMakeLists.txt" ]; then
    echo "Please run this script from the root of the robotraconteur repo directory."
    exit 1
fi

mkdir -p build_gen_src
cd build_gen_src
cmake -DPACKAGE_SWIG_SOURCE_ALL=ON -DBUILD_PYTHON3=ON -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DUSE_PREGENERATED_SOURCE=OFF $CMAKE_ARGS ..
cmake --build . --config=Release --target package_source_swig
cp -r generated_src ..
cd ..
rm -r build_gen_src
