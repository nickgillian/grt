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

namespace GRT{

class MinDistModel
{
public:
    
	/**
	 Default Constructor
	 */
	MinDistModel();
    
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
	
	bool train(UINT classLabel,MatrixDouble &trainingData,UINT numClusters);
	double predict(const VectorDouble &observation);
	void recomputeThresholdValue();
	
	UINT getClassLabel() const;
	UINT getNumFeatures() const;
	UINT getNumClusters() const;
	UINT getDistanceMode() const;
    double getRejectionThreshold() const;
    double getGamma() const;
    double getTrainingMu() const;
    double getTrainingSigma() const;
    MatrixDouble getClusters() const;
    
    bool setClassLabel(UINT classLabel);
    bool setClusters(MatrixDouble &clusters);
    bool setGamma(double gamma);
    bool setRejectionThreshold(double rejectionThreshold);
    bool setTrainingSigma(double trainingSigma);
    bool setTrainingMu(double trainingMu);
    
private:
    double SQR(double x){ return x*x; }
	UINT classLabel;
	UINT numFeatures;
	UINT numClusters;
	double rejectionThreshold;			//The classification threshold value
	double gamma;				//The number of standard deviations to use for the threshold
	double trainingMu;			//The average confidence value in the training data
	double trainingSigma;		//The simga confidence value in the training data
	MatrixDouble clusters;
};

} //End of namespace GRT

#endif //GRT_MINDISTMODEL_HEADER

