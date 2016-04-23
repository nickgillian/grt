##Build
This folder contains a CMakeLists.txt that can be used to generate a makefile for building the GRT (as a shared or static library) and also for building the GRT examples.

##Supported Operating Systems
This CMakeLists file has been tested on Windows, Linux (Ubuntu 14.04), and OSX (10.9).

##C++11
The default build of the GRT uses C++11, you should therefore ensure you add C++11 support to any project using the GRT.

If it is difficult to add C++11 to your setup, then you can still build the by disabling GRT C++11 functionality (note, this will limit the full functionality of the toolkit).

To disable C++11 support, you can pass the following option to CMake when building the GRT:

```
$ cmake .. -DENABLE_CXX11_SUPPORT=OFF
```

If you want to enable C++11 support for the GRT, but are building the toolkit without using CMake, then you should define the following before including the main GRT header:

```C++
//Enable C++11 support
#define GRT_CXX11_ENABLED

//Include the main GRT header for the first time
#include <GRT/GRT.h>
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Call a C++ 11 function, this will be > 0 if C++11 is enabled
    cout << "Thread pool size: " << ThreadPool::getThreadPoolSize() << endl;

    return EXIT_SUCCESS;
}
```

##Build Instructions

- [Linux Build Instructions](#linux-build-instructions)
- [OSX Build Instructions](#osx-build-instructions)
- [Windows Build Instructions](#windows-build-instructions)

##Linux Build Instructions

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

- if you want to build the library as a static library (instead of shared) and define a custom install prefix, then add the following options:

```
$ cmake .. -DBUILD_SHARED_LIB=OFF -DCMAKE_INSTALL_PREFIX=/usr/local
```

- compile the GRT library and examples (j controls the number of cores you want to use, N == the number of cores):

```
$ make -jN
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


##OSX Build Instructions

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

- if you want to build the library as a static library (instead of shared) and define a custom install prefix, then add the following options:

```
$ cmake .. -DBUILD_SHARED_LIB=OFF -DCMAKE_INSTALL_PREFIX=/usr/local
```

- compile the GRT library and examples (j controls the number of cores you want to use, N == the number of cores):

```
$ make -jN
```
	
- you can then install the GRT by running:

```
$ sudo make install
```
	
- the GRT examples will be: build/examples 

## Linux/OSX Linking Instructions
After installing the GRT, you can build a new project and link against it in OS X or Linux by:

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

##Windows Build Instructions

Note, you will need to download and install the cmake Windows installer (http://www.cmake.org/) and a compiler that supports C++11 (such as VisualStudio 2015).

To build the GRT library on windows as a static library and compile the examples:

- open terminal and cd to this build directory

- create a temporary build folder: 

```
$ mkdir tmp
```
	
- cd to the temporary build folder:

```
$ cd tmp
```
	
- possibly define some environment variables that will be used by cmake:

```
$ REM Create projects that build their files in parallel.
$ set CFLAGS=/MP
$ set CXXFLAGS=/MP

$ REM Ignore warnings about DLL linking.
$ set LDFLAGS=/ignore:4049,4217

```

- use cmake to generate the Visual Studio project for your machine:

```
$ cmake ..
```
	
- this will generate project files for all the GRT classes and examples in the temporary build directory

- open the main project file with Visual Studio, which should be called ALL_BUILD.vcxproj.

- use Visual Studio to compile the GRT library and examples (shortcut in Visual Studio should be F7)

- if this is successfull, it should build the GRT as a static library (grt.lib) and build all the GRT examples

- you can now either run the pre-built GRT examples, or use the GRT static library in your custom projects (see [Visual Studio Linking Instructions](#visual-studio-linking-instructions) for more details)


## Visual Studio Linking Instructions
After building the GRT as a static library, you can build a new custom project and link against the GRT library by:

- create a new Visual Studio C++ project

- add some custom code to your project, such as the example code below:

```C++
#include <GRT.h>
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Print the GRT version
    cout << "GRT Version: " << GRTBase::getGRTVersion() << endl;

    return EXIT_SUCCESS;
}
```

- select the project in the Visual Studio 'Solution' window, right click and select 'Properties'
- select the 'C/C++', 'General', 'Additional Include Directories' and add the path to the folder containing the GRT source code and main GRT header (GRT.h)
- select the 'Linker','General', 'Additional Library Directories' and add the path to the folder containing the GRT static library (grt.lib, this should be in a folder called Debug in your temporary build directory)
- select the 'Linker','Input', 'Additional Dependencies' and add the GRT static library (grt.lib)
- click 'Apply', you should now be able to build your custom project and link against the GRT

##Contributors
Thanks to:

- Romain Guillemot for creating the original CMake file and for improving the support for Windows!
- Leslie Zhai for improving the build for Linux users!
