cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake"
    CACHE FILEPATH
    "Toolchain to use for building this package and dependencies")

include(CMakeDependentOption)
include(HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.20.46.tar.gz"
    SHA1 "62f6c01e51dbd15ae46e85c23413e74da88b6a3b"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(jaeger-struct VERSION 0.0.0)

hunter_add_package(Protobuf)
find_package(Protobuf CONFIG REQUIRED)

add_executable(protoc-gen-jaeger-struct src/jaeger-struct/CGenerator.cpp)
target_include_directories(protoc-gen-jaeger-struct PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_link_libraries(protoc-gen-jaeger-struct PUBLIC
  protobuf::libprotobuf protobuf::libprotoc)
