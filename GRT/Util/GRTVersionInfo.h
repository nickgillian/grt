/*
GRT MIT License
Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef GRT_VERSION_INFO_HEADER
#define GRT_VERSION_INFO_HEADER

#define GRT_VERSION_MAJOR 0
#define GRT_VERSION_MINOR 2
#define GRT_VERSION_PATCH 4
#define GRT_VERSION "0.2.4"
#define GRT_REVISION "4-Dec-2016"

//Workout which platform we are using
#if defined( __WIN32__ ) || defined( _WIN32 ) || defined(WIN32)
	#define __GRT_WINDOWS_BUILD__
#endif

#if defined(__APPLE_CC__) || defined(__APPLE__)
    #define __GRT_OSX_BUILD__
#endif

#if defined(__linux__)
    #define __GRT_LINUX_BUILD__
#endif

#endif //GRT_VERSION_INFO_HEADER
