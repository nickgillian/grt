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

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GRT DEFAULT_MSG GRT_LIBRARIES GRT_INCLUDE_DIRS)