# This is an example Makefile for building a custom cpp file and linking it against the GRT library.
# Note, this directory also includes a GRT CMakeLists file, which can be used to generate a custom makefile for your specific machine and is recommend over this hand coded makefile.
# This Makefile only works on Linux/OSX, you should use the CMakeLists file with CMake to generate a custom make file for Windows
# To build the example using this Makefile, install make (http://www.gnu.org/software/make/) then:
# (1) First, you need to build and install the GRT library, you can do this by following the instructions in the build folder
# (2) Open terminal and cd to the directory containing this file
# (3) type: make
# (4) make will now build the example
# (5) To run the example, run: ./main

GRT_HEADERS = -I/usr/local/include
LIB_DIR = -L/usr/local/lib
LIBS = -lgrt
CC = g++
CFLAGS = -g -O2 -Wall $(LIB_DIR)

#Build the example (linking against the GRT library)
make: $(GRT_EXAMPLES_SRC) $(GRT_SRC) $(GRT_LIB)
	@echo "----------------- Building GRT Example -----------------"
    	#Compile the example code to create the object files
	$(CC) -c main.cpp $(GRT_HEADERS) $(CFLAGS)
    	#Compile the actual output examples and link against the GRT library
	$(CC) main.o -o main $(GRT_HEADERS) $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *.out
