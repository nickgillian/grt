##Build
This folder contains a CMakeLists.txt that can be used to generate a makefile for building the GRT (as a shared or static library) and also for building the GRT examples.

##Supported Operating Systems
This CMakeLists file has been tested on Windows, Linux (Ubuntu 14.04), and OSX (10.9).

##C++11
As of GRT version 1.0 revision 03-04-15, the toolkit now uses C++11. You should therefore ensure you add C++11 support to any project using the GRT.

##Build Instructions
Note, you will need to install make (http://www.gnu.org/software/make/) and cmake (http://www.cmake.org/) and a compiler that supports C++11 (such as g++ 4.7 and later).

You can build the GRT as a shared library and compile the examples examples by:

- open terminal and cd to this build directory

- create a temporary build folder: 

```
$ mkdir tmp
```
	
- cd to the temporary build folder:

```
$ cd tmp
```
	
- use cmake to generate the makefile for your machine:

```
$ cmake ..
```

- if you want to define the install prefix:

```
$ cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

	
- if you want to build the library as a static library (instead of shared), add the following option:

```
$ cmake .. -DBUILD_STATIC_LIB=ON
```

- compile the GRT library and examples (j controls the number of cores you want to use):

```
$ make -j 2
```
	
- you can then install the GRT by running:

```
$ sudo make install
```

- if you want to setup pkgconfig for Linux machines:

```
$ pkg-config --cflags --libs grt
```
	
- the GRT examples will be: build/examples 

##Linking Instructions
After installing the GRT, you can build a new project and link against it by:

```
$ g++ -std=c++11 -c main.cpp -I/usr/local/include
$ g++ main.o -o main -I/usr/local/include -L/usr/local/lib -lgrt
	
//Run the application
$ ./main
```

where main.cpp is:

```C++
#include <GRT/GRT.h>
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Print the GRT version
    cout << "GRT Version: " << GRTBase::getGRTVersion() << endl;

    return EXIT_SUCCESS;
}
```

##Contributors
Thanks to:

- Romain Guillemot for creating the original CMake file and for improving the support for Windows!
- Leslie Zhai for improving the build for Linux users!
