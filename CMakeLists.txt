cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "c:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()



project(GameServerFramework VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_WIN32_WINNT=0x0601)

set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dependencies
# list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED_DIR}/x64-windows")
# message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
# message(STATUS "VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")
find_package(asio CONFIG REQUIRED)
find_package(Protobuf REQUIRED)






# Subdirectories
add_subdirectory(Framework)
add_subdirectory(GameShared)
add_subdirectory(Database)
add_subdirectory(Module/Protobuf)
add_subdirectory(Server)
add_subdirectory(Client)
add_subdirectory(examples)

