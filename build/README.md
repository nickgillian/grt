##Build
This folder contains a CMakeLists.txt that can be used to generate a makefile for building the GRT (as a static library) and also for building the GRT examples.

##Supported Operating Systems
This CMakeLists file has been tested on Windows, Linux (Ubuntu 14.04), and OSX (10.9).

##Build Instructions
Note, you will need to install make (http://www.gnu.org/software/make/) and cmake (http://www.cmake.org/). 

You can build the GRT as a static library and compile the examples examples by:
- open terminal and cd to this build directory
- create a temporary build folder: 

    $ mkdir tmp
- cd to the temporary build folder:

    $ cd tmp
	
- use cmake to generate the makefile for your machine:

    $ cmake ..
	
- compile the GRT library and examples (j controls the number of cores you want to use):

    $ make -j 2
	
- you can then install the GRT by running:

    $ sudo make install
	
- by default, this should install the GRT headers to: /usr/local/include/GRT
- the GRT static library (libgrt.a) should be installed to: /usr/local/lib
- the GRT examples will be: build/examples 
