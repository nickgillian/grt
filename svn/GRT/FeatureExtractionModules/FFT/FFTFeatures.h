/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements the FFTFeatures featue extraction module.
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

#ifndef GRT_FFT_FEATURES_HEADER
#define GRT_FFT_FEATURES_HEADER

#include "../../CoreModules/FeatureExtraction.h"
#include "FFT.h"

namespace GRT{

class FFTFeatures : public FeatureExtraction
{
public:
    /**
     Default Constructor, sets the default settings for the FFTFeatures module.
	 The fftWindowSize and numChannelsInFFTSignal parameters should match the settings of the FFT module that will be used as input to this module.
	 
	 @param UINT fftWindowSize: the window size of the FFT that will be used as input to this module. Default value = 512
	 @param UINT numChannelsInFFTSignal: this is the number of channels (i.e. input dimensions) to the FFT module.  Default value = 1
	 @param bool computeMaxFreqFeature: sets if the maximum frequency feature will be included in the feature vector. Default value = true
	 @param bool computeMaxFreqSpectrumRatio: sets if the maximum-frequency spectrum-frequency ratio feature will be included in the feature vector. Default value = true
	 @param bool computeCentroidFeature: sets if the centroid frequency feature will be included in the feature vector. Default value = true
	 @param bool computeTopNFreqFeatures: sets if the top N frequency feature will be included in the feature vector. Default value = true
	 @param bool N: sets if size of N for the top N frequency features. Default value = 10
     */
	FFTFeatures(UINT fftWindowSize=512,UINT numChannelsInFFTSignal=1,bool computeMaxFreqFeature = true,bool computeMaxFreqSpectrumRatio = true,bool computeCentroidFeature = true,bool computeTopNFreqFeatures = true,UINT N = 10);
    
    /**
     Copy Constructor, copies the FFTFeatures from the rhs instance to this instance
     
	 @param const FFTFeatures &rhs: another instance of the FFTFeatures class from which the data will be copied to this instance
     */
    FFTFeatures(const FFTFeatures &rhs);
    
    /**
     Default Destructor
     */
	virtual ~FFTFeatures(void);
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const FFTFeatures &rhs: another instance of the FFTFeatures class from which the data will be copied to this instance
	 @return a reference to this instance of FFT
     */
    FFTFeatures& operator=(const FFTFeatures &rhs);

    /**
     Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
     This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
     This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeline.
     
	 @param FeatureExtraction *featureExtraction: a pointer to another instance of an FFTFeatures, the values of that instance will be cloned to this instance
	 @return true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
     Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     
	 @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
	 @return true if the data was processed, false otherwise
     */
    virtual bool computeFeatures(const VectorDouble &inputVector);
    
    /**
     Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the FFTFeatures by re-initiliazing the instance.
     
	 @return true if the FFTFeatures was reset, false otherwise
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
     Initializes the FFTFeatures. 
     Should be called before calling the computeFFT(...) or computeFeatures(...) methods.
     This function is automatically called by the constructor.
     
	 @param UINT fftWindowSize: the window size of the FFT that will be used as input to this module. Default value = FFT::FFT_WINDOW_SIZE_512
	 @param UINT numChannelsInFFTSignal: the size of the FFT feature vector that will be used as input to this module.  Default value = 1
	 @param bool computeMaxFreqFeature: sets if the maximum frequency feature will be included in the feature vector. Default value = true
	 @param bool computeMaxFreqSpectrumRatio: sets if the maximum-frequency spectrum-frequency ratio feature will be included in the feature vector. Default value = true
	 @param bool computeCentroidFeature: sets if the centroid frequency feature will be included in the feature vector. Default value = true
	 @param bool computeTopNFreqFeatures: sets if the top N frequency feature will be included in the feature vector. Default value = true
	 @param bool N: sets if size of N for the top N frequency features. Default value = 10
	 @return true if the FTT was initialized, false otherwise
     */   
    bool init(UINT fftWindowSize,UINT numChannelsInFFTSignal,bool computeMaxFreqFeature,bool computeMaxFreqSpectrumRatio,bool computeCentroidFeature,bool computeTopNFreqFeatures,UINT N);
    
    //Tell the compiler we are using the following functions from the FeatureExtraction class to stop hidden virtual function warnings
    
protected:
    UINT fftWindowSize;
    UINT numChannelsInFFTSignal;
    bool computeMaxFreqFeature;
    bool computeMaxFreqSpectrumRatio;
    bool computeCentroidFeature;
    bool computeTopNFreqFeatures;
    
    UINT N;
    double maxFreqFeature;
    double maxFreqSpectrumRatio;
    double centroidFeature;
    VectorDouble topNFreqFeatures;
    
    static RegisterFeatureExtractionModule< FFTFeatures > registerModule;
    
public:
};

} //End of namespace GRT

#endif //GRT_FFT_FEATURES_HEADER

