# Defina os arquivos fonte para a biblioteca ou executável
set(SOURCE_FILES
    estrutura.hpp
    libModbusMaestecPLC.cpp
    libModbusMaestecPLC.hpp
    libModbusSystematomSPU.cpp
    libModbusSystematomSPU.hpp
    main.cpp
    TrigaServer.cpp
    TrigaServer.hpp
)

# Crie a biblioteca ou executável
add_executable(TrigaServer ${SOURCE_FILES})

# Inclua os headers diretamente
#target_include_directories(TrigaServer PRIVATE ../vendor/libModbusMaestecPLC ../vendor/libModbusSystematomSPU)

# Inclua os arquivos diretamente
#target_sources(TrigaServer PRIVATE ../vendor/libModbusMaestecPLC/libModbusMaestecPLC.cpp
#                                   ../vendor/libModbusSystematomSPU/libModbusSystematomSPU.cpp)

target_link_libraries(TrigaServer PRIVATE -lmodbus -lserialport -ljsoncpp)

