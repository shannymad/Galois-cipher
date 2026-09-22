cmake_minimum_required(VERSION 3.15)
project(galois_stream_cipher CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_library(galois INTERFACE)
target_include_directories(galois INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/galois
)
add_executable(galois_cli src/main.cpp)
target_link_libraries(galois_cli PRIVATE galois)

enable_testing()
add_subdirectory(tests)
