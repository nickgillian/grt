
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

if (APPLE) # Assume homebrew
    set(PYTHONINTERP_FOUND YES)
    set(PYTHON_EXECUTABLE /usr/local/bin/python3)
    if (NOT EXISTS ${PYTHON_EXECUTABLE})
        message(FATAL_ERROR "Cannot find homebrew Python v3 in ${PYTHON_EXECUTABLE}")
    endif()
    message(STATUS "Found Python 3 executable: ${PYTHON_EXECUTABLE}")
else()
    find_package(PythonInterp 3)
endif()
find_package(Numpy REQUIRED)
if (NOT IS_DIRECTORY ${NUMPY_INCLUDE_DIR})
    message(FATAL_ERROR "Cannot find Numpy")
endif()
include_directories(${NUMPY_INCLUDE_DIR})


include_directories(${GRT_SRC_DIR})

add_compile_options(-Wno-deprecated)

set_property(SOURCE GRT.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE GRT.i PROPERTY SWIG_FLAGS -Wall -verbose -py3)
swig_add_library(GRT LANGUAGE python SOURCES GRT.i)

swig_link_libraries(GRT ${PYTHON_LIBRARIES} ${GRT_LIB_NAME} )

# Copy python examples files
file(GLOB PYTHON_EXAMPLES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.py"
        )
file(COPY ${PYTHON_EXAMPLES} DESTINATION ".")
