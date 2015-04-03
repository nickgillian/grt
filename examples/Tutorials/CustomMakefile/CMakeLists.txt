cmake_minimum_required(VERSION 2.8)

project(GRT_PROJECT_EXAMPLE C CXX)

##Add the main GRT library
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(grt REQUIRED)
include_directories(${GRT_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GRT_LIBRARIES})

#Add any custom OS definitions
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
 
#Add the custom files to be compiled
add_executable (main main.cpp)

#Link the custom application against the GRT library
target_link_libraries (main ${LIBS})

