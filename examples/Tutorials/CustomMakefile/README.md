####Custom Makefile

This directory contains two examples of how to compile your own C++ project and link against the GRT library.

There are two ways to do this:
- Using make, with a hand written Makefile
- Using CMake, to generate a custom Makefile for your machine, then using make to build this custom project

We recommend using CMake, as this should build a better makefile for your OS without having to customize your own makefile.

Note, that for both these options you need to first compile and install the GRT library.  Follow the instructions in the main GRT build folder to install the library.

##Building a sample project using make
To build the sample main.cpp file using make, install make on your machine, then open terminal and cd to this directory and run:

    $ make

This should build the project in the current directory, you can then run the sample application via:

    $ ./main

##Building a sample project using cmake
To build the sample main.cpp file using Cmake, install Cmake on your machine, then open terminal and cd to this directory and run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

This should build the project in the build directory, you can then run the sample application via:

    $ ./main
