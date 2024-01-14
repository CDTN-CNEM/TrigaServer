cmake_minimum_required(VERSION 3.10)
project(TrigaServer)

set(CMAKE_CXX_STANDARD 23)

add_executable(TrigaServer main.cpp)

add_library(libModbusMaestecPLC STATIC libModbusMaestecPLC.cpp libModbusMaestecPLC.hpp)
add_library(libModbusSystematomSPU STATIC libModbusSystematomSPU.cpp libModbusSystematomSPU.hpp)

target_link_libraries(TrigaServer libModbusMaestecPLC libModbusSystematomSPU serialport modbus)
