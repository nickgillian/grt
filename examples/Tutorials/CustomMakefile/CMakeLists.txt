cmake_minimum_required(VERSION 2.8)

project(GRT C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(grt REQUIRED)
include_directories(${GRT_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GRT_LIBRARIES})
 
add_executable (main main.cpp)
target_link_libraries (main ${LIBS})

