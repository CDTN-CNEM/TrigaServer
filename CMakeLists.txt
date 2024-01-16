cmake_minimum_required(VERSION 3.14)

include(FetchContent)

project(TrigaServer)

set(CMAKE_CXX_STANDARD 20)
cmake_policy(SET CMP0079 NEW)

find_package(PkgConfig REQUIRED)
find_package(Boost 1.81 REQUIRED COMPONENTS system)

pkg_search_module(SYSTEMD "systemd")

pkg_search_module(cxxopts REQUIRED IMPORTED_TARGET cxxopts)
pkg_search_module(Modbus REQUIRED IMPORTED_TARGET libmodbus)
pkg_search_module(SerialPort REQUIRED IMPORTED_TARGET libserialport)
pkg_search_module(jsoncpp REQUIRED IMPORTED_TARGET jsoncpp)

FetchContent_Declare(
  libModbusSystematomSPU
  GIT_REPOSITORY https://github.com/CDTNuclear/libModbusSystematomSPU
  GIT_TAG 673a638f5f19ced2ba7d3a71895edc17225cde48)

FetchContent_Declare(
  libModbusMaestecPLC
  GIT_REPOSITORY https://github.com/CDTNuclear/libModbusMaestecPLC
  GIT_TAG 51d18e459b6caeed108caa61c1885c727a91724c)

FetchContent_MakeAvailable(libModbusSystematomSPU libModbusMaestecPLC)

set(TRIGASERVER_SRC
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/TrigaServer.cpp
    ${libmodbussystematomspu_SOURCE_DIR}/libModbusSystematomSPU.cpp
    ${libmodbusmaestecplc_SOURCE_DIR}/libModbusMaestecPLC.cpp)

add_executable(trigaserver ${TRIGASERVER_SRC})

target_include_directories(
  trigaserver
  PRIVATE ${PROJECT_SOURCE_DIR}/src
          ${libmodbussystematomspu_SOURCE_DIR}
          ${libmodbusmaestecplc_SOURCE_DIR}
          ${Boost_INCLUDE_DIRS}
          ${CXXOPTS_INCLUDE_DIRS}
          ${MODBUS_INCLUDE_DIRS}
          ${SERIALPORT_INCLUDE_DIRS}
          ${JSONCPP_INCLUDE_DIRS})

target_link_libraries(
  trigaserver PRIVATE ${Boost_LIBRARIES} PkgConfig::cxxopts PkgConfig::Modbus
                      PkgConfig::SerialPort PkgConfig::jsoncpp)

target_compile_options(
  trigaserver PRIVATE ${CXXOPTS_CFLAGS_OTHER} ${MODBUS_CFLAGS_OTHER}
                      ${SERIALPORT_CFLAGS_OTHER} ${JSONCPP_CFLAGS_OTHER})

install(TARGETS trigaserver)
install(FILES ${PROJECT_SOURCE_DIR}/resources/TrigaServer.conf
        DESTINATION /etc/)

if(SYSTEMD_FOUND AND "${SYSTEMD_SERVICES_INSTALL_DIR}" STREQUAL "")
  execute_process(
    COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemdsystemunitdir systemd
    OUTPUT_VARIABLE SYSTEMD_SERVICES_INSTALL_DIR)
  string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_SERVICES_INSTALL_DIR
                       "${SYSTEMD_SERVICES_INSTALL_DIR}")
  configure_file(${PROJECT_SOURCE_DIR}/resources/triga-server.service
                 ${CMAKE_BINARY_DIR}/triga-server.service)
  configure_file(${PROJECT_SOURCE_DIR}/resources/TrigaServer.conf
                 ${CMAKE_BINARY_DIR}/TrigaServer.conf)
                 
  install(FILES ${CMAKE_BINARY_DIR}/triga-server.service
          DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR})
  install(FILES ${CMAKE_BINARY_DIR}/TrigaServer.conf
          DESTINATION /etc/)
  
endif()
