##Build
This folder contains a CMakeLists.txt that can be used to generate a makefile for building the GRT (as a shared or static library) and also for building the GRT examples.

##Supported Operating Systems
This CMakeLists file has been tested on Windows, Linux (Ubuntu 14.04), and OSX (10.9).

##Build Instructions
Note, you will need to install make (http://www.gnu.org/software/make/) and cmake (http://www.cmake.org/). 

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

- pkgconfig

```
$ pkg-config --cflags --libs grt
```
	
- the GRT examples will be: build/examples 

##Linking Instructions
After installing the GRT, you can build a new project and link against it by:

```
$ g++ -c main.cpp -I/usr/local/include
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
