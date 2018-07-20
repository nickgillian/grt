
if (APPLE) # Assume homebrew
    file(GLOB SWIG_EXECUTABLE /usr/local/Cellar/swig/*/bin/swig)
    if (NOT EXISTS ${SWIG_EXECUTABLE})
        message(FATAL_ERROR "Cannot find homebrew SWIG v3 in /usr/local/Cellar/swig/")
    endif()
    message(STATUS "Found swig executable: ${SWIG_EXECUTABLE}")
endif()
find_package(SWIG 3 REQUIRED)
INCLUDE(${SWIG_USE_FILE})

find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
message(STATUS "Found python: ${PYTHON_INCLUDE_DIRS} ${PYTHON_LIBRARIES}")

#find_package(PythonInterp 3)
set(PYTHONINTERP_FOUND YES)
set(PYTHON_EXECUTABLE /usr/local/bin/python3)
find_package(Numpy REQUIRED)
include_directories(${NUMPY_INCLUDE_DIR})

include_directories(${GRT_SRC_DIR})

add_compile_options(-Wno-deprecated)

set_property(SOURCE GRT.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE GRT.i PROPERTY SWIG_FLAGS -Wall -verbose -py3)
swig_add_library(GRT LANGUAGE python SOURCES GRT.i)

swig_link_libraries(GRT ${PYTHON_LIBRARIES} ${GRT_LIB_NAME} )

add_custom_command(
        TARGET ${SWIG_MODULE_GRT_REAL_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/hello_world_example.py" # source
        "hello_world_example.py" # destination
        COMMENT "Copying hello_world_example.py"
)
