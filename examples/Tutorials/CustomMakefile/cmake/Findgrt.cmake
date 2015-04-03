# - Try to find Gesture Recognition Toolkit library
# Once done, this will define
#
#  GRT_FOUND - system has GRT
#  GRT_INCLUDE_DIRS - the GRT include directories
#  GRT_LIBRARIES - link these to use GRT

FIND_LIBRARY(GRT_LIBRARY
	NAMES libgrt.so grt.so libgrt.dylib grt.dylib
	PATHS
		${GRT_LIBRARIES}
)

FIND_PATH(GRT_INCLUDE_DIR
	NAMES GRT/GRT.h
	PATHS
		${GRT_INCLUDE_DIRS}
		${GRT_INCLUDE_DIR}
)

MARK_AS_ADVANCED(
  GRT_LIBRARY
  GRT_INCLUDE_DIR
)
SET(GRT_INCLUDE_DIRS "${GRT_INCLUDE_DIR}")
SET(GRT_LIBRARIES "${GRT_LIBRARY}")

#Add any custom OSX definitions
if(APPLE)
    #Add C++11 and multithreading support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
else()
	
#Add any custom Linux definitions
if(UNIX) 
    #Add C++11 and multithreading support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
endif()
endif() #end of else(APPLE)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GRT DEFAULT_MSG GRT_LIBRARIES GRT_INCLUDE_DIRS)
