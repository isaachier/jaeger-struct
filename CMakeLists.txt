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

include(CheckCXXCompilerFlag)
include(CTest)

hunter_add_package(Protobuf)
find_package(Protobuf CONFIG REQUIRED)

check_cxx_compiler_flag(-Wall have_wall)
if(have_wall)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

check_cxx_compiler_flag(-Werror have_werror)
if(have_werror)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

add_library(runtime
  src/jaeger-struct/runtime/list.c
  src/jaeger-struct/runtime/string.c)
target_include_directories(runtime PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

add_library(compiler
  src/jaeger-struct/compiler/Field.cpp
  src/jaeger-struct/compiler/Generator.cpp
  src/jaeger-struct/compiler/Strings.cpp)
target_include_directories(compiler PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_link_libraries(compiler PUBLIC
  protobuf::libprotobuf protobuf::libprotoc)

add_executable(protoc-gen-jaeger_struct
  src/jaeger-struct/compiler/Main.cpp)
target_link_libraries(protoc-gen-jaeger_struct PUBLIC compiler)

if(BUILD_TESTING)
  hunter_add_package(GTest)
  find_package(GTest CONFIG REQUIRED)
  add_executable(UnitTest
    src/jaeger-struct/compiler/StringsTest.cpp)
  target_compile_definitions(UnitTest PUBLIC
      GTEST_HAS_TR1_TUPLE=0
      GTEST_USE_OWN_TR1_TUPLE=0)
  target_link_libraries(UnitTest PUBLIC
    compiler GTest::main)
  add_test(NAME UnitTest COMMAND UnitTest)
endif()
