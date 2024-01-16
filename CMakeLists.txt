# Defina a versão mínima do CMake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)#C++20: atomic_smart_pointers (std::atomic<std::shared_ptr>)
cmake_policy(SET CMP0079 NEW)

# Define o nome do seu projeto
project(TrigaServer)

# Adiciona os subdiretórios ao projeto
add_subdirectory(src)
