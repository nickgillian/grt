/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class acts as the main interface for using a Hidden Markov Model.
 
 @remark This implementation is based on Rabiner, Lawrence. "A tutorial on hidden Markov models and selected applications in speech recognition." Proceedings of the IEEE 77.2 (1989): 257-286.
 */

/**
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

#ifndef GRT_HMM_ENUMS_HEADER
#define GRT_HMM_ENUMS_HEADER

#include <iostream>
    
enum HMMModelTypes{HMM_ERGODIC=0,HMM_LEFTRIGHT};
enum HMMTypes{HMM_DISCRETE=0,HMM_CONTINUOUS};

#endif //GRT_HMM_ENUMS_HEADER
