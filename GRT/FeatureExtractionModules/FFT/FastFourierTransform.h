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

/*
 This code is based on Dominic Mazzoni's FFT c++ wrapper, which is based on a free FFT implementation
 by Don Cross (http://www.intersrv.com/~dcross/fft.html) and the FFT algorithms from Numerical Recipes.
 */

#ifndef GRT_FAST_FOURIER_TRANSFORM_HEADER
#define GRT_FAST_FOURIER_TRANSFORM_HEADER

#include "../../Util/GRTCommon.h"
#include "../../CoreModules/GRTBase.h"

namespace GRT{

class FastFourierTransform : public GRTBase{
	
public:
		
	FastFourierTransform();
    
	FastFourierTransform(const FastFourierTransform &rhs);
    
	virtual ~FastFourierTransform();
    
	FastFourierTransform& operator=(const FastFourierTransform &rhs);
    
	bool init(const unsigned int windowSize,const unsigned int windowFunction = RECTANGULAR_WINDOW,const bool computeMagnitude = true,const bool computePhase = true,bool enableZeroPadding = true);
    
    bool computeFFT( VectorDouble &data );
    
	VectorDouble getMagnitudeData();
	VectorDouble getPhaseData();
	VectorDouble getPowerData();
	double getAveragePower();
	double *getMagnitudeDataPtr();
	double *getPhaseDataPtr();
	double *getPowerDataPtr();
    
	UINT getFFTSize(){ return windowSize; }
    
protected:
    bool windowData( VectorDouble &data );
    bool realFFT( const VectorDouble &realIn, double *realOut, double *imagOut );
    bool FFT(int NumSamples,bool InverseTransform,double *realIn, double *imagIn, double *realOut, double *imagOut);
    int numberOfBitsNeeded(int PowerOfTwo);
    int reverseBits(int index, int NumBits);
    void initFFT();
    inline int fastReverseBits(const int i, const int NumBits);
    inline bool isPowerOfTwo(const unsigned int x);
    
    unsigned int windowSize;
    unsigned int windowFunction;
    bool initialized;
    bool computeMagnitude;
    bool computePhase;
	bool enableZeroPadding;
    VectorDouble fftReal;
    VectorDouble fftImag;
    VectorDouble tmpReal;
    VectorDouble tmpImag;
    VectorDouble magnitude;
    VectorDouble phase;
    VectorDouble power;
    double averagePower;
    const static int MAX_FAST_BITS = 16;
    vector< vector< int > > bitTable;
    
public:
    enum WindowFunctionOptions{RECTANGULAR_WINDOW=0,BARTLETT_WINDOW,HAMMING_WINDOW,HANNING_WINDOW};

};
    
}//End of namespace GRT

#endif //GRT_FAST_FOURIER_TRANSFORM_HEADER
