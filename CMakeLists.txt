cmake_minimum_required(VERSION 3.10)

project(C_SFML_Compiler VERSION 1.0 LANGUAGES CXX)

# Estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ✅ Indicar dónde está SFMLConfig.cmake
set(SFML_DIR "C:/SFML-2.6.1/lib/cmake/SFML")

# Buscar SFML
find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

# Pasar a subdirectorio src
add_subdirectory(src)
