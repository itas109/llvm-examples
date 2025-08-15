cmake_minimum_required (VERSION 3.15)

project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    antlr4
    URL "${PROJECT_SOURCE_DIR}/../../antlr4-cpp-runtime-4.13.2-source-patched.zip"
    URL_HASH "SHA256=bf7689a8ad167ae51832edb24ca0405deb3e47c4d103c659687cdec6ca46b3dd"
)
set(ANTLR_BUILD_CPP_TESTS OFF CACHE BOOL "" FORCE) # disable test
set(ANTLR_BUILD_SHARED OFF CACHE BOOL "" FORCE) # shared library
set(ANTLR_BUILD_STATIC ON CACHE BOOL "" FORCE) # static library
set(WITH_STATIC_CRT OFF CACHE BOOL "" FORCE) # MultiThreadedDLL
FetchContent_MakeAvailable(antlr4)

include_directories(generated) # lexer parser headers

file(GLOB ANTLR4_GENERATE_SRC ${PROJECT_SOURCE_DIR}/generate/*.cpp)

add_executable(${PROJECT_NAME} main.cpp  ${ANTLR4_GENERATE_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${antlr4_SOURCE_DIR}/runtime/src) # antlr4-runtime.h
if(ANTLR_BUILD_SHARED)
add_dependencies(${PROJECT_NAME} antlr4_shared)
target_link_libraries(${PROJECT_NAME} PRIVATE antlr4_shared)
else()
add_dependencies(${PROJECT_NAME} antlr4_static)
target_link_libraries(${PROJECT_NAME} PRIVATE antlr4_static)
endif()