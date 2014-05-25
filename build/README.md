##Build
This folder contains an example Makefile for building the GRT (as a static library) and also for building the GRT examples.

##Supported Operating Systems
This Makefile currently works on Linux/OSX.  There is currently limited Windows support, however this is a work in progress.

##Build Instructions
To build the GRT and examples, install make (http://www.gnu.org/software/make/) and then:
- Open terminal and cd to the directory containing this file
- type: **make**
- make will now build the GRT library (libgrt.a) and also build the GRT examples (which will be placed in the local folder examples)
- To run any of the examples, cd into the examples folder and then run: **./ExampleName.out**
- If you just want to build the GRT as a library to use in your own projects, without building the examples, then type: **make grtlib**
