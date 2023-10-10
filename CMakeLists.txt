cmake_minimum_required(VERSION 3.10)

project(xkbpointer CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mtune=native -march=native")

find_package(X11 REQUIRED)
message("X11_FOUND: ${X11_FOUND}")
message("X11_Xtst_FOUND: ${X11_Xtst_FOUND}")
message("X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}")
message("X11_Xtst_INCLUDE_PATH: ${X11_Xtst_INCLUDE_PATH}")
message("X11_LIBRARIES: ${X11_LIBRARIES}")
message("X11_Xtst_LIB: ${X11_Xtst_LIB}")
link_libraries(${X11_LIBRARIES} ${X11_Xtst_LIB})
include_directories(${X11_INCLUDE_DIR} ${X11_Xtst_INCLUDE_PATH})

set(SOURCE_FILES main.cpp xkbpointer.cpp)
add_executable(xkbpointer ${SOURCE_FILES})
