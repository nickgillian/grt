/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements the MinDist classifier algorithm.
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

#ifndef GRT_MINDISTMODEL_HEADER
#define GRT_MINDISTMODEL_HEADER

#include "../../Util/GRTCommon.h"
#include "../../ClusteringModules/KMeans/KMeans.h"

GRT_BEGIN_NAMESPACE

class GRT_API MinDistModel
{
public:
    
	/**
	 Default Constructor
	 */
	MinDistModel();
    
    /**
     Default Copy Constructor
     */
    MinDistModel(const MinDistModel &rhs);
    
    /**
     Default Destructor
     */
	~MinDistModel(void);
    
    /**
     Defines how the data from the rhs MinDistModel should be copied to this MinDistModel
     
     @param const MinDistModel &rhs: another instance of a MinDistModel
     @return returns a pointer to this instance of the MinDistModel
     */
	MinDistModel &operator=(const MinDistModel &rhs);
	
	bool train(UINT classLabel,MatrixFloat &trainingData,UINT numClusters,Float minChange,UINT maxNumEpochs);
	Float predict(const VectorFloat &observation);
	void recomputeThresholdValue();
	
	UINT getClassLabel() const;
	UINT getNumFeatures() const;
	UINT getNumClusters() const;
	UINT getDistanceMode() const;
    Float getRejectionThreshold() const;
    Float getGamma() const;
    Float getTrainingMu() const;
    Float getTrainingSigma() const;
    MatrixFloat getClusters() const;
    
    bool setClassLabel(UINT classLabel);
    bool setClusters(MatrixFloat &clusters);
    bool setGamma(Float gamma);
    bool setRejectionThreshold(Float rejectionThreshold);
    bool setTrainingSigma(Float trainingSigma);
    bool setTrainingMu(Float trainingMu);
    bool setTrainingLoggingEnabled(bool enabled);
    
private:
    Float SQR(const Float &x){ return (x*x); }
    bool trainingLoggingEnabled;
	UINT classLabel;
	UINT numFeatures;
	UINT numClusters;
	Float rejectionThreshold;			//The classification threshold value
	Float gamma;				//The number of standard deviations to use for the threshold
	Float trainingMu;			//The average confidence value in the training data
	Float trainingSigma;		//The simga confidence value in the training data
	MatrixFloat clusters;
};

GRT_END_NAMESPACE

#endif //GRT_MINDISTMODEL_HEADER

