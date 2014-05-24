# Gesture Recognition Toolkit (GRT)

The Gesture Recognition Toolkit (GRT) is a cross-platform, open-source, C++ machine learning library 
that has been specifically designed for real-time gesture recognition.

##Building the GRT

You can find a makefile in the GRTBuild folder that will compile the GRT as a static library (on Linux and OSX systems) and also
compile all the example projects. See the README file in the GRTBuild folder for more info.

If you want to directly include the source code in your own C++ projects, then you can do this without using the makefile. See
Installing and using the GRT in your C++ projects section for more information on this.

##Installing and using the GRT in your C++ projects

To use the GRT you simply need to add all of the code contained in the GRT folder to your c++ project.  
If you are using an IDE such as XCode or VisualStudio then you can either add the files in the GRT 
folder to your project using "File -> Add Files to project", or in most IDEs you can simply drag the 
GRT folder (from Finder or Windows Explorer) into your IDE which should add all of the files in the GRT 
folder to your project.

After you have added the code in the GRT folder to your project, you can now use any of the GRT 
functions or classes by adding the following two lines of code to the top of any header file in your 
project in which you want to use the GRT code:

    #include "GRT/GRT.h"
    using namespace GRT;

The first line of code above adds the main GRT header file (GRT.h) to your project, this header file 
contains all of the GRT module header files which means you do not need to manually include any other 
GRT header files.  The second line of code states that you are using the GRT namespace, this means
that you don't have to write GRT::WhatEverClass every time you want to use a GRT class and can instead
simply write WhatEverClass.

Note that, depending on your IDE, you may have to specify the physical path that 
you placed the GRT folder in on your hard drive, for instance you might have to write something like:

    #include "/Users/Nick/Documents/Dev/cpp/gesture-recognition-toolkit/GRT/GRT.h"

##Tutorials and Examples

You can find a large number of tutorials and examples in the GRTExamples folder.  You can also find a
wide range of examples and references on the main GRT wiki:

http://www.nickgillian.com/wiki/pmwiki.php?n=GRT.GestureRecognitionToolkit

##License

The Gesture Recognition Toolkit is available under a MIT license.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
