#!/bin/bash

rm -rf lib*

git clone https://github.com/CDTNuclear/libModbusSystematomSPU.git
git clone https://github.com/CDTNuclear/libModbusMaestecPLC.git

cd ../src/

ln -s ../vendor/libModbusMaestecPLC/libModbusMaestecPLC.cpp       libModbusMaestecPLC.cpp
ln -s ../vendor/libModbusMaestecPLC/libModbusMaestecPLC.hpp       libModbusMaestecPLC.hpp
ln -s ../vendor/libModbusSystematomSPU/libModbusSystematomSPU.hpp libModbusSystematomSPU.hpp
ln -s ../vendor/libModbusSystematomSPU/libModbusSystematomSPU.cpp libModbusSystematomSPU.cpp 
