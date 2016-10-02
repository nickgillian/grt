/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
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

#ifndef GRT_DTW_HEADER
#define GRT_DTW_HEADER

#include "../../CoreModules/Classifier.h"
#include "../../Util/TimeSeriesClassificationSampleTrimmer.h"

GRT_BEGIN_NAMESPACE

class GRT_API IndexDist{
    public:
    IndexDist(int x=0,int y=0,Float dist=0){
        this->x = x;
        this->y = y;
        this->dist = dist;
    }
    ~IndexDist(){};
    IndexDist& operator=(const IndexDist &rhs){
        if(this!=&rhs){
            this->x = rhs.x;
            this->y = rhs.y;
            this->dist = rhs.dist;
        }
        return (*this);
    }
    
    int x;
    int y;
    Float dist;
};

///////////////// DTW Template /////////////////
class GRT_API DTWTemplate{
    public:
    DTWTemplate(){
        classLabel = 0;
        trainingMu = 0.0;
        trainingSigma = 0.0;
        averageTemplateLength=0;
    }
    ~DTWTemplate(){};
    
    UINT classLabel;                    //The class that this template belongs to
    MatrixFloat timeSeries;            //The raw time series
    Float trainingMu;                  //The mean distance value of the training data with the trained template
    Float trainingSigma;               //The sigma of the distance value of the training data with the trained template
    UINT averageTemplateLength;          //The average length of the examples used to train this template
};

/**
@brief This class implements Dynamic Time Warping.  Dynamic Time Warping (DTW) is a powerful classifier that
works very well for recognizing temporal gestures. Temporal gestures can be defined as a cohesive sequence of
movements that occur over a variable time period.  The DTW algorithm is a supervised learning algorithm that
can be used to classify any type of N-dimensional, temporal signal. The DTW algorithm works by creating a
template time series for each gesture that needs to be recognized, and then warping the realtime signals to
each of the templates to find the best match. The DTW algorithm also computes rejection thresholds that enable
the algorithm to automatically reject sensor values that are not the K gestures the algorithm has been trained
to recognized (without being explicitly told during the prediction phase if a gesture is, or is not, being performed).
You can find out more about the DTW algorithm in <a href="http://www.nickgillian.com/papers/Gillian_NDDTW.pdf">Gillian, N. (2011) Recognition of multivariate temporal musical gestures using n-dimensional dynamic time warping</a>.

The DTW algorithm is part of the GRT classification modules.

@remark This implementation is based on <a href="http://www.nickgillian.com/papers/Gillian_NDDTW.pdf">Gillian, N. (2011) Recognition of multivariate temporal musical gestures using n-dimensional dynamic time warping</a>.

@example ClassificationModulesExamples/DTWExample/DTWExample.cpp
*/
class GRT_API DTW : public Classifier
{
public:

    enum DistanceMethods{ABSOLUTE_DIST=0,EUCLIDEAN_DIST,NORM_ABSOLUTE_DIST};
    enum RejectionModes{TEMPLATE_THRESHOLDS=0,CLASS_LIKELIHOODS,THRESHOLDS_AND_LIKELIHOODS};
    
    /**
    Default Constructor
    
    @param useScaling: sets if the training and prediction data should be scaled to a specific range.  Default value is useScaling = false
    @param useNullRejection: sets if null rejection will be used for the realtime prediction.  If useNullRejection is set to true then the predictedClassLabel will be set to 0 (which is the default null label) if the distance between the inputVector and the closest template is greater than the null rejection threshold for the top predicted class.  The null rejection threshold is computed for each class during the training phase. Default value is useNullRejection = false
    @param nullRejectionCoeff: sets the null rejection coefficient, this is a multipler controlling the null rejection threshold for each class.  This will only be used if the useNullRejection parameter is set to true.  Default value is nullRejectionCoeff = 3.0
    @param rejectionMode: sets the method used for null rejection. The options are TEMPLATE_THRESHOLDS, CLASS_LIKELIHOODS or THRESHOLDS_AND_LIKELIHOODS.  Default = TEMPLATE_THRESHOLDS
    @param dtwConstrain: sets if the DTW warping path should be constrained within a specific distance from the main radius of the cost matrix.  Default value = true
    @param radius: controls the radius of the warping path, which is used if the dtwConstrain is set to true. Should be a value between [0 1]. Default value = 0.2
    @param offsetUsingFirstSample: sets if each timeseries should be offset by the first sample in the timeseries. Default value = false
    @param useSmoothing: sets if the input timeseries should be smoothed (i.e. averaged and downsampled). Default value = false
    @param smoothingFactor: controls the amount of downsampling if the useSmoothing parameter is set to true. Default value = 5
    @param nullRejectionLikelihoodThreshold: set the null rejection threshold for likelihoods when CLASS_LIKELIHOODS or THRESHOLDS_AND_LIKELIHOODS modes are used for rejectionMode. Default value = 0.99
    */
    DTW(bool useScaling=false,bool useNullRejection=false,Float nullRejectionCoeff=3.0,UINT rejectionMode = DTW::TEMPLATE_THRESHOLDS,bool dtwConstrain=true,Float radius=0.2,bool offsetUsingFirstSample=false,bool useSmoothing = false,UINT smoothingFactor = 5, Float nullRejectionLikelihoodThreshold = 0.99);
    
    /**
    Default copy constructor
    
    Defines how the data from the rhs DTW should be copied to this DTW
    
    @param rhs: another instance of a DTW
    */
    DTW(const DTW &rhs);
    
    /**
    Default Destructor
    */
    virtual ~DTW(void);
    
    /**
    Defines how the data from the rhs DTW should be copied to this DTW
    
    @param rhs: another instance of a DTW
    @return returns a pointer to this instance of the DTW
    */
    DTW& operator=(const DTW &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an DTW instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another DTW instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the DTW model, using the labelled timeseries classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the DTW model was trained, false otherwise
    */
    virtual bool train_(TimeSeriesClassificationData &trainingData);
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input vector to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    This predicts the class of the timeseries.
    This overrides the predict function in the Classifier base class.
    
    @param timeSeries: the input timeseries to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(MatrixFloat &timeSeries);
    
    /**
    This resets the DTW classifier.
    
    @return returns true if the DTW model was successfully reset, false otherwise.
    */
    virtual bool reset();
    
    /**
    This overrides the clear function in the Classifier base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This saves the trained DTW model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the DTW model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained DTW model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the DTW model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This recomputes the null rejection thresholds for each of the classes in the DTW model.
    This will be called automatically if the setGamma(Float gamma) function is called.
    The DTW model needs to be trained first before this function can be called.
    
    @return returns true if the null rejection thresholds were updated successfully, false otherwise
    */
    virtual bool recomputeNullRejectionThresholds();
    
    /**
    This returns the number of templates in the current model.
    
    @return returns an unsigned integer representing the number of templates in the current model.
    */
    UINT getNumTemplates() const { return numTemplates; }
    
    /**
    Sets the rejection mode used for null rejection. The rejection mode should be one of the RejectionModes enums.
    
    @return returns true if the rejection mode was updated successfully, false otherwise
    */
    bool setRejectionMode(UINT rejectionMode);
    
    /**
    Sets the rejection threshold for rejections based on class likelihood.
    
    @return returns true if the rejection threshold was updated successfully, false otherwise
    */
    bool setNullRejectionThreshold(Float nullRejectionLikelihoodThreshold);
    
    /**
    Sets if each timeseries should be offset by the first sample in the timeseries.
    This can help the DTW algorithm be more invariant to the position a gesture is performed without having to normalize the data.
    You shoud call this function before training the algorithm.
    
    @param offsetUsingFirstSample: sets if each timeseries should be offset by the first sample in the timeseries
    @return returns true if the offsetUsingFirstSample parameter was updated successfully, false otherwise
    */
    bool setOffsetTimeseriesUsingFirstSample(bool offsetUsingFirstSample);
    
    /**
    Sets if the warping path should be constrained to within a specific radius from the main diagonal of the cost matrix.
    
    @param constrain: if true then the warping path should be constrained
    @return returns true if the constrain parameter was updated successfully, false otherwise
    */
    bool setContrainWarpingPath(bool constrain);
    
    /**
    Sets the warping radius, this is used to constrain the warping path within a specific radius from the main diagonal of the cost matrix.
    This is only used if the #constrainWarpingPath parameter is set to true.
    The radius should be a value between [0 1].
    
    @param radius: the new warping radius (should be between [0 1]
    @return returns true if the warping radius parameter was updated successfully, false otherwise
    */
    bool setWarpingRadius(Float radius);
    
    /**
    Gets the rejection mode used for null rejection. The rejection mode will be one of the RejectionModes enums.
    
    @return returns an unsigned integer representing the current rejection mode
    */
    UINT getRejectionMode() const { return rejectionMode; }
    
    /**
    Sets if z-normalization should be used for both training and realtime prediction.  This should be called before training the templates.
    
    @param useZNormalization: if true then the data will be z-normalized before both training and prediction
    @param constrainZNorm: if true then the data will only be z-normalized if the std-dev of the data is below a specific threshold
    @return returns true if z-normalization was updated successfully, false otherwise
    */
    bool enableZNormalization(bool useZNormalization,bool constrainZNorm = true);
    
    /**
    Sets if the training data should be trimmed before training the DTW templates.  If set to true then any training samples that have very little movement at the start or end of
    a file will be trimmed so as to remove the none movement at the start or end of the file.  This can be useful if some of your examples have a section were the user paused before
    performing a gesture, or were the recording continued for a few seconds after the gesture ended.  The trimThreshold and maximumTrimPrecentage parameters control how extreme the
    trimming can be.  The trimThreshold should be in a range of [0 1] and the maximumTrimPrecentage should be in a range of [0 100].  If the amount of energy in the timeseries is below
    the trimThreshold then it will be trimmed.
    
    Note that if the trimming mode is enabled, and a time series sample exceeds the maximumTrimPercentage, then that sample will not be
    added to the modified (trimmed) training dataset. If any sample exceeds the maximumTrimPercentage then a warning message will be
    displayed via the DTW trainingLog (unless the trainingLog's logging has been disabled).
    
    @param trimTrainingData: if true, then the training examples will be trimmed prior to training the DTW templates
    @param trimThreshold: the trimThreshold sets the threshold at which the data should be trimmed. This should be in a range of [0 1]
    @param maximumTrimPercentage: this sets the maximum amount of data that can be trimmed for one example. This should be in the range of [0 100]
    @return returns true if the trimTrainingData parameters were updated successfully, false otherwise
    */
    bool enableTrimTrainingData(bool trimTrainingData,Float trimThreshold,Float maximumTrimPercentage);
    
    /**
    Gets the DTW models.
    
    @return returns a vector of DTW templates, or an empty vector if no model has been trained.
    */
    Vector< DTWTemplate > getModels() const { return templatesBuffer; }
    
    /**
    Sets the DTW models, overwriting any previous models. The size of the new templates vector must match the size of the old templates buffer.
    
    @return returns true if the templates were updated, false otherwise
    */
    bool setModels( Vector< DTWTemplate > newTemplates );
    
    /**
    Gets the current data in the DTW circular buffer.
    
    @return returns a vector of VectorFloats containing the current data in the DTW circular buffer
    */
    Vector< VectorFloat > getInputDataBuffer() const { return continuousInputDataBuffer.getData(); }
    
    /**
    Gets the distances matrices from the last prediction.  Each element in the vector represents the distance matrices for each corresponding class.
    
    @return returns a vector of MatrixFloat containing the distance matrices from the last prediction, or an empty vector if no prediction has been made
    */
    const Vector< MatrixFloat >& getDistanceMatrices() const { return distanceMatrices; }
    
    /**
    Gets the warping paths from the last prediction.  Each element in the vector represents the warping path for each corresponding class.
    
    @return returns a vector of vectors containing the warping paths from the last prediction, or an empty vector if no prediction has been made
    */
    const Vector< Vector< IndexDist > >& getWarpingPaths() const { return warpPaths; }

    /**
    Gets a string that represents the DTW class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;
    
protected:
    //Public training and prediction methods
    bool train_NDDTW(TimeSeriesClassificationData &trainingData,DTWTemplate &dtwTemplate,UINT &bestIndex);
    
    //The actual DTW function
    Float computeDistance(MatrixFloat &timeSeriesA,MatrixFloat &timeSeriesB,MatrixFloat &distanceMatrix,Vector< IndexDist > &warpPath);
    Float d(int m,int n,MatrixFloat &distanceMatrix,const int M,const int N);
    Float inline MIN_(Float a,Float b, Float c);
    
    //Scaling and Utility Functions
    void scaleData(TimeSeriesClassificationData &trainingData);
    void scaleData(MatrixFloat &data,MatrixFloat &scaledData);
    void znormData(TimeSeriesClassificationData &trainingData);
    void znormData(MatrixFloat &data,MatrixFloat &normData);
    void smoothData(VectorFloat &data,UINT smoothFactor,VectorFloat &resultsData);
    void smoothData(MatrixFloat &data,UINT smoothFactor,MatrixFloat &resultsData);
    void offsetTimeseries(MatrixFloat &timeseries);
    bool loadLegacyModelFromFile( std::fstream &file );
    
    Vector< DTWTemplate > templatesBuffer;      //A buffer to store the templates for each time series
    Vector< MatrixFloat > distanceMatrices;
    Vector< Vector< IndexDist > > warpPaths;
    CircularBuffer< VectorFloat > continuousInputDataBuffer;
    UINT                numTemplates;           //The number of templates in our buffer
    UINT                rejectionMode;          //The rejection mode used to reject null gestures during the prediction phase
    
    //Flags
    bool                useSmoothing;           //A flag to check if we need to smooth the data
    bool                useZNormalisation;      //A flag to check if we need to znorm the training and prediction data
    bool                offsetUsingFirstSample; //A flag to check if each timeseries should be offset by the first sample in the time series
    bool                constrainZNorm;         //A flag to check if we need to constrain zNorm (only zNorm if stdDev > zNormConstrainThreshold)
    bool                constrainWarpingPath;   //A flag to check if we need to constrain the dtw cost matrix and search
    bool                trimTrainingData;       //A flag to check if we need to trim the training data first before training
    
    Float               zNormConstrainThreshold;//The threshold value to be used if constrainZNorm is turned on
    Float              radius;
    Float               trimThreshold;          //Sets the threshold under which training data should be trimmed (default 0.1)
    Float               maximumTrimPercentage;  //Sets the maximum amount of data that can be trimmed for each training sample (default 20)
    Float               nullRejectionLikelihoodThreshold; //Sets the threshold for null rejection based on likelihoods
    
    UINT                smoothingFactor;        //The smoothing factor if smoothing is used
    UINT                distanceMethod;         //The distance method to be used (should be of enum DISTANCE_METHOD)
    UINT                averageTemplateLength;  //The overall average template length (over all the templates)
    
private:
    static RegisterClassifierModule< DTW > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_DTW_HEADER
    