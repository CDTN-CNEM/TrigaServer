cmake_minimum_required(VERSION 3.10)
project(TrigaServer)

# Compile a biblioteca TrigaModbus
add_library(TrigaModbus STATIC TrigaModbus.cpp)

# Adicione os arquivos de inclusão para a biblioteca TrigaModbus
target_include_directories(TrigaModbus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Compile o programa TrigaServer
add_executable(TrigaServer TrigaServer.cpp)

# Vincule a biblioteca TrigaModbus ao programa TrigaServer
target_link_libraries(TrigaServer PRIVATE TrigaModbus)