/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief The FFT class computes the Fourier transform of an N dimensional signal using a Fast Fourier Transform algorithm.
 
 The FFT class computes N independent FFTs, one for each dimension in the input signal. 
 The FFT is computed using the previous M samples input to the FFT instance, where M is equal to the FFT Window Size parameter, 
 which is set by the user when they initialize the FFT.
 The FFT Window Size parameter must be a power of two (16,32,64,128,256,512,1024,2048,4096,etc.). Note that, because of the symmetries
 within the FFT computation, the results of an FFT will be half the FFT Window Size. This means that if you create an FFT with a Window Size
 of 256, the resulting vectors that you will get (such as the magnitude and phase) will each have 128 elements in them.
 
 For each N dimensional input signal, the FFT will output:
 - An M dimensional vector containing the magnitude of each frequency component
 - An M dimensional vector containing the phase of each frequency component
 where M is equal to the FFTWindowSize / 2 . These values will be concatenated into one vector, in the order of
 { [magnitude dimension 1] [phase dimension 1] [magnitude dimension 2] [phase dimension 2] ... [magnitude dimension N] [phase dimension N] }. 
 The size of this concatenated output vector will therefore be equal to N * M * 2 - where N is the number of input dimensions to the FFT, M is the FFTWindowSize/2, 
 and 2 represents the magnitude and phase vectors. If you only really need the magnitude or phase of a signal, as opposed to both, then you can turn off the computation 
 and concatenation of the element you do not need to save unnecessary computations and memory copies, this can either be done in the FFT's constructor or by using the 
 setComputeMagnitude(bool computeMagnitude) and setComputePhase(bool computePhase) functions.
 
 @example FeatureExtractionModules/FFTExample/FFTExample.cpp
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

#ifndef GRT_FFT_HEADER
#define GRT_FFT_HEADER

#include "../../CoreModules/FeatureExtraction.h"
#include "FastFourierTransform.h"

namespace GRT{

class FFT : public FeatureExtraction
{
public:
    /**
     Constructor, sets the fftWindowSize, hopSize, fftWindowFunction, if the magnitude and phase should be computed during the FFT and the number
     of dimensions in the input signal.
     
     @param UINT fftWindowSize: sets the size of the fft, this should be a power of two. Default fftWindowSize=512
     @param UINT hopSize: sets how often the fft should be computed. If the hopSize parameter is set to 1 then the FFT will be computed everytime
     the classes computeFeatures(...) or computeFFT(...) functions are called. You may not want to compute the FFT of the input signal for every
     sample however, if this is the case then set the hopSize parameter to N, in which case the FFT will only be computed every N samples on the previous M values, where M is equal to the fftWindowSize. Default hopSize=1
     @param UINT numDimensions: the dimensionality of the input data to the FFT.  Default numDimensions = 1
     @param UINT fftWindowFunction: sets the window function of the FFT. This should be one of the FFTWindowFunctionOptions enumeration values. Default windowFunction=RECTANGULAR_WINDOW
     @param bool computeMagnitude: sets if the magnitude (and power) of the spectrum should be computed on the results of the FFT. Default computeMagnitude=true
     @param bool computePhase: sets if the phase of the spectrum should be computed on the results of the FFT. Default computePhase=true
     */
	FFT(UINT fftWindowSize=512,UINT hopSize=1,UINT numDimensions=1,UINT fftWindowFunction=RECTANGULAR_WINDOW,bool computeMagnitude=true,bool computePhase=true);
    
    /**
     Copy Constructor, copies the FFT from the rhs instance to this instance
     
	 @param const FFT &rhs: another instance of the FFT class from which the data will be copied to this instance
     */
    FFT(const FFT &rhs);
    
    /**
     Default Destructor
     */
	virtual ~FFT(void);
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const FFT &rhs: another instance of the FFT class from which the data will be copied to this instance
	 @return a reference to this instance of FFT
     */
    FFT& operator=(const FFT &rhs);

    /**
     Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
     This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
     This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeline.
     
	 @param FeatureExtraction *featureExtraction: a pointer to another instance of an FFT, the values of that instance will be cloned to this instance
	 @return true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
     Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the FFT's computeFFT(...) function.
     
	 @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
	 @return true if the data was processed, false otherwise
     */
    virtual bool computeFeatures(const VectorDouble &inputVector);
    
    /**
     Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the FFT by re-initiliazing the instance.
     
	 @return true if the FFT was reset, false otherwise
     */    
    virtual bool reset();
    
    /**
     This saves the feature extraction settings to a file.
     
     @param const string filename: the filename to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(const string filename) const;
    
    /**
     This saves the feature extraction settings to a file.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool loadModelFromFile(const string filename);
    
    /**
     This saves the feature extraction settings to a file.
     This overrides the saveSettingsToFile function in the FeatureExtraction base class.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads the feature extraction settings from a file.
     This overrides the loadSettingsFromFile function in the FeatureExtraction base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     Initializes the FFT. 
     Should be called before calling the computeFFT(...) or computeFeatures(...) methods.
     This function is automatically called by the constructor.
     
     @param UINT fftWindowSize: sets the size of the fft, this must be a power of two
     @param UINT hopSize: sets how often the fft should be computed. If the hopSize parameter is set to 1 then the FFT will be computed everytime
     the classes computeFeatures(...) or computeFFT(...) functions are called. You may not want to compute the FFT of the input signal for every
     sample however, if this is the case then set the hopSize parameter to N, in which case the FFT will only be computed every N samples on the previous M values, where M is equal to the fftWindowSize
     @param UINT numDimensions: the dimensionality of the input data to the FFT
     @param UINT windowFunction: sets the window function of the FFT. This should be one of the WindowFunctionOptions enumeration values
     @param bool computeMagnitude: sets if the magnitude (and power) of the spectrum should be computed on the results of the FFT
     @param bool computePhase: sets if the phase of the spectrum should be computed on the results of the FFT
	 @return true if the FTT was initialized, false otherwise
     */   
    bool init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT windowFunction,bool computeMagnitude,bool computePhase);
    
    /**
     Computes the FFT of the previous M input samples, where M is the size of the fft window set by the constructor.
     The FFT of the input will only be computed if the current hop counter value matches the hopSize.
     This function should only be used if the dimensionality of the FFT has been set to 1.
     
     @param const double x: the new sample, this will be added to a buffer and the FFT will be computed for the data in the buffer
	 @return true if the FTT was updated successfully, false otherwise
     */   
    bool update(const double x);
    
    /**
     Computes the FFT of the previous M input samples, where M is the size of the fft window set by the constructor.
     The FFT of the input will only be computed if the current hop counter value matches the hopSize.
     The dimensionality of the input vector must match the number of dimensions for the FFT.
     
     @param const VectorDouble &x: the new N-dimensional sample, this will be added to a buffer and the FFT will be computed for the data in the buffer
	 @return true if the FTT was updated successfully, false otherwise
     */   
    bool update(const VectorDouble &x);
    
    /**
     Returns the current hopSize.
     
	 @return returns the current hopSize value if the FFT has been initialized, otherwise zero will be returned
     */   
    UINT getHopSize();
    
    /**
     Returns the current dataBufferSize, which is the same as the FFT window size.
     
	 @return returns the current dataBufferSize value if the FFT has been initialized, otherwise zero will be returned
     */  
    UINT getDataBufferSize();
    
    /**
     Returns the FFT window size.
     
	 @return returns the current FFT window size value if the FFT has been initialized, otherwise zero will be returned
     */  
    UINT getFFTWindowSize();
    
    /**
     Returns the current FFT window function enumeration value.
     
	 @return returns the current FFT window function value if the FFT has been initialized, otherwise zero will be returned
     */  
    UINT getFFTWindowFunction();
    
    /**
     Returns the current hop counter value.
     
	 @return returns the current hop counter value if the FFT has been initialized, otherwise zero will be returned
     */  
    UINT getHopCounter();
    
    /**
     Returns if the magnitude (and power) of the FFT spectrum is being computed.
     
	 @return true if the magnitude (and power) of the FFT spectrum is being computed and if the FFT has been initialized, otherwise false will be returned
     */   
    bool getComputeMagnitude(){ if(initialized){ return computeMagnitude; } return false; }
    
    /**
     Returns if the phase of the FFT spectrum is being computed.
     
	 @return true if the phase of the FFT spectrum is being computed and if the FFT has been initialized, otherwise false will be returned
     */   
    bool getComputePhase(){ if(initialized){ return computePhase; } return false; }
    
    /**
     Returns the FFT results computed from the last FFT of the input signal.
     
	 @return returns a vector of FastFourierTransform (where the size of the vector is equal to the number of input dimensions for the FFT).  An empty vector will be returned if the FFT was not computed
     */
    vector< FastFourierTransform > getFFTResults(){ return fft; }
    
    /**
     Returns a pointer to the FFT results computed from the last FFT of the input signal.
     
	 @return returns a pointer to the vector of FastFourierTransform (where the size of the vector is equal to the number of input dimensions for the FFT).  An empty vector will be returned if the FFT was not computed
     */
    vector< FastFourierTransform >& getFFTResultsPtr(){ return fft; }
    
    VectorDouble getFrequencyBins(const unsigned int sampleRate);
    
    /**
     Sets the hopSize parameter, this sets how often the fft should be computed. 
     If the hopSize parameter is set to 1 then the FFT will be computed everytime the classes' computeFeatures(...) or computeFFT(...) functions are called. 
     You may not want to compute the FFT of the input signal for every sample however, if this is the case then set the hopSize parameter to N, in which case the FFT will only be computed every N samples on the previous M values, where M is equal to the fftWindowSize. 
     The hopSize must be greater than zero.
     Setting the hopSize will also reset the hop counter.
     
     @param UINT hopSize: the new hopSize parameter, must be greater than zero
	 @return returns true if the hopSize parameter was successfully updated, false otherwise
     */
    bool setHopSize(UINT hopSize);
    
    /**
     Sets the fftWindowSize parameter, this sets the size of the fft, this must be a power of two.
     Setting this value will also re-initialize the FFT.
     
     @param UINT fftWindowSize: the new fftWindowSize parameter, this must be a power of two.
	 @return returns true if the fftWindowSize parameter was successfully updated, false otherwise
     */
    bool setFFTWindowSize(UINT fftWindowSize);
    
    /**
     Sets the fftWindowFunction parameter, this should be one of the FFTWindowFunctionOptions enumeration values.
     
     @param UINT fftWindowFunction: the new fftWindowFunction parameter, must be one of the FFTWindowFunctionOptions enumeration values
	 @return returns true if the fftWindowFunction parameter was successfully updated, false otherwise
     */
    bool setFFTWindowFunction(UINT fftWindowFunction);
    
    /**
     Sets if the magnitude (and power) of the FFT spectrum should be computed.
     
     @param bool computeMagnitude: the new computeMagnitude parameter
	 @return returns true if the computeMagnitude parameter was successfully updated, false otherwise
     */
    bool setComputeMagnitude(bool computeMagnitude);
    
    /**
     Sets if the phase of the FFT spectrum should be computed.
     
     @param bool computePhase: the new computeMagnitude parameter
	 @return returns true if the computePhase parameter was successfully updated, false otherwise
     */
    bool setComputePhase(bool computePhase);
    
protected:
    bool isPowerOfTwo(UINT x);                                   ///< A helper function to compute if the input is a power of two
    bool validateFFTWindowFunction(UINT fftWindowFunction);

    UINT hopSize;                                               ///< The current hopSize, this sets how often the fft should be computed
    UINT dataBufferSize;                                        ///< Stores how much previous input data is stored in the dataBuffer
    UINT fftWindowSize;                                         ///< Stores the size of the fft (and also the dataBuffer)
    UINT fftWindowFunction;                                     ///< The current windowFunction used for the FFT
    UINT hopCounter;                                            ///< Keeps track of how many input samples the FFT has seen
    bool computeMagnitude;                                      ///< Tracks if the magnitude (and power) of the FFT need to be computed
    bool computePhase;                                          ///< Tracks if the phase of the FFT needs to be computed
    GRT::VectorDouble tempBuffer;                               ///< A temporary buffer used to store the input data for the FFT
    CircularBuffer< VectorDouble > dataBuffer;                  ///< A circular buffer used to store the previous M inputs
    vector< FastFourierTransform > fft;                         ///< A buffer used to store the FFT results
    std::map< unsigned int, unsigned int > windowSizeMap;       ///< A map to relate the FFTWindowSize enumerations to actual values
    
    static RegisterFeatureExtractionModule< FFT > registerModule;
    
public:
    enum FFTWindowFunctionOptions{RECTANGULAR_WINDOW=0,BARTLETT_WINDOW,HAMMING_WINDOW,HANNING_WINDOW};
};

} //End of namespace GRT

#endif //GRT_FFT_HEADER
