# This is an example Makefile for building a custom cpp file and linking it against the GRT static library.
# This Makefile only works on Linux/OSX, Windows support will be added soon.
# To build the example, install make (http://www.gnu.org/software/make/) and then:
# (1) First, you need to build the GRT static library, you can do this by following the instructions in the build folder
# (2) Open terminal and cd to the directory containing this file
# (3) type: make
# (4) make will now build the example
# (5) To run the example, run: ./main.out

GRT_HEADERS = -I../../../GRT
LIB_DIR = -L../../../build
LIBS = -lgrt
CC = g++
CFLAGS = -g -O2 -Wall $(LIB_DIR)

#Build the example (linking against the GRT library)
make: $(GRT_EXAMPLES_SRC) $(GRT_SRC) $(GRT_LIB)
	@echo "----------------- Building GRT Examples -----------------"
    	#Compile the example code to create the object files
	$(CC) -c main.cpp $(GRT_HEADERS) $(CFLAGS)
    	#Compile the actual output examples (which all end in .out) and link against the GRT library
	$(CC) main.o -o main.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)

clean:
	rm -f *.o *.out
