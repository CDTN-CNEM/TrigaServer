#!/bin/bash

clean() {
    rm -rf build lib*
    
    git clone https://github.com/CDTNuclear/libModbusSystematomSPU.git
    git clone https://github.com/CDTNuclear/libModbusMaestecPLC.git

    ln -s libModbusMaestecPLC/libModbusMaestecPLC.cpp       libModbusMaestecPLC.cpp
    ln -s libModbusMaestecPLC/libModbusMaestecPLC.hpp       libModbusMaestecPLC.hpp
    ln -s libModbusSystematomSPU/libModbusSystematomSPU.hpp libModbusSystematomSPU.hpp
    ln -s libModbusSystematomSPU/libModbusSystematomSPU.cpp libModbusSystematomSPU.cpp
}

set -x
cd src

if [ "$1" == "clean" ]; then
    clean
fi

mkdir -p build
cd build
cmake ..
make

cp -f TrigaServer ../../
