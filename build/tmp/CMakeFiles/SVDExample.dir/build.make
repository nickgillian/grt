# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/lukas/.biicode_env/cmake-3.0.2-Linux-64/bin/cmake

# The command to remove a file.
RM = /home/lukas/.biicode_env/cmake-3.0.2-Linux-64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lukas/Dokumente/BA/GRT/build

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lukas/Dokumente/BA/GRT/build/tmp

# Include any dependencies generated for this target.
include CMakeFiles/SVDExample.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SVDExample.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SVDExample.dir/flags.make

CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o: CMakeFiles/SVDExample.dir/flags.make
CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o: /home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lukas/Dokumente/BA/GRT/build/tmp/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o -c /home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp

CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp > CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.i

CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp -o CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.s

CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.requires:
.PHONY : CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.requires

CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.provides: CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.requires
	$(MAKE) -f CMakeFiles/SVDExample.dir/build.make CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.provides.build
.PHONY : CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.provides

CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.provides.build: CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o

# Object files for target SVDExample
SVDExample_OBJECTS = \
"CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o"

# External object files for target SVDExample
SVDExample_EXTERNAL_OBJECTS =

SVDExample: CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o
SVDExample: CMakeFiles/SVDExample.dir/build.make
SVDExample: libgrt.a
SVDExample: CMakeFiles/SVDExample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable SVDExample"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SVDExample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SVDExample.dir/build: SVDExample
.PHONY : CMakeFiles/SVDExample.dir/build

CMakeFiles/SVDExample.dir/requires: CMakeFiles/SVDExample.dir/home/lukas/Dokumente/BA/GRT/examples/UtilExamples/SVDExample/SVDExample.cpp.o.requires
.PHONY : CMakeFiles/SVDExample.dir/requires

CMakeFiles/SVDExample.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SVDExample.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SVDExample.dir/clean

CMakeFiles/SVDExample.dir/depend:
	cd /home/lukas/Dokumente/BA/GRT/build/tmp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lukas/Dokumente/BA/GRT/build /home/lukas/Dokumente/BA/GRT/build /home/lukas/Dokumente/BA/GRT/build/tmp /home/lukas/Dokumente/BA/GRT/build/tmp /home/lukas/Dokumente/BA/GRT/build/tmp/CMakeFiles/SVDExample.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SVDExample.dir/depend

