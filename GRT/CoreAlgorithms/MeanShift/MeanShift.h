
/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements the MeanShift clustering algorithm.
 
 @remark This implementation is based on http://en.wikipedia.org/wiki/Mean-shift
 
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

#ifndef GRT_MEAN_SHIFT_HEADER
#define GRT_MEAN_SHIFT_HEADER

#include "../../CoreModules/MLBase.h"

GRT_BEGIN_NAMESPACE

class MeanShift : public MLBase{
public:
    MeanShift() : MLBase("MeanShift") {
    }
    
    virtual ~MeanShift(){
        
    }
    
    bool search( const VectorFloat &meanStart, const Vector< VectorFloat > &points, const Float searchRadius, const Float sigma = 20.0 ){
        
        //clear the results from any previous search
        clear();
        
        const unsigned int numDimensions = (unsigned int)meanStart.size();
        const unsigned int numPoints = (unsigned int)points.size();
        const Float gamma = 1.0 / (2 * SQR(sigma) );
        unsigned int iteration = 0;
        VectorFloat numer(2,0);
        VectorFloat denom(2,0);
        VectorFloat kernelDist(2,0);
        Float pointsWithinSearchRadius = 0;
        
        mean = meanStart;
        VectorFloat lastMean = mean;
        
        //Start the search loop
        while( true ){
            
            //Reset the counters
            pointsWithinSearchRadius = 0;
            std::fill(numer.begin(),numer.end(),0);
            std::fill(denom.begin(),denom.end(),0);
            std::fill(kernelDist.begin(),kernelDist.end(),0);
            
            //Update the numerator and denominator for points that are with the search radius
            for(unsigned int i=0; i<numPoints; i++){
                
                //Compute the distance of the current point to the mean
                Float distToMean = euclideanDist( mean, points[i] );
                
                //If the point is within the search radius then update numer and denom
                if( distToMean < searchRadius ){
                    
                    for(unsigned int j=0; j<numDimensions; j++){
                        kernelDist[j] = gaussKernel( points[i][j], mean[j], gamma );
                        numer[j] += kernelDist[j] * points[i][j];
                        denom[j] += kernelDist[j];
                    }
                    
                    pointsWithinSearchRadius++;
                }
            }
            
            //Update the mean
            Float change = 0;
            for(unsigned int j=0; j<numDimensions; j++){
                
                mean[j] = numer[j] / denom[j];
                
                change += grt_sqr( mean[j] - lastMean[j] );
                
                lastMean[j] = mean[j];
            }
            change = grt_sqrt( change );
            
            trainingLog << "iteration: " << iteration;
            trainingLog << " mean: ";
            for(unsigned int j=0; j<numDimensions; j++){
                trainingLog << mean[j] << " ";
            }
            trainingLog << " change: " << change << std::endl;
            
            if( change < minChange ){
                trainingLog << "min changed limit reached - stopping search" << std::endl;
                break;
            }
            
            if( ++iteration >= maxNumEpochs ){
                trainingLog << "max number of iterations reached - stopping search." << std::endl;
                break;
            }
            
        }
        numTrainingIterationsToConverge = iteration;
        trained = true;
        
        return true;
    }
    
    VectorFloat getMean() const {
        return mean;
    }
    
    Float gaussKernel( const Float &x, const Float &mu, const Float gamma ){
        return exp( gamma * grt_sqr(x-mu) );
    }
    
    Float gaussKernel( const VectorFloat &x, const VectorFloat &mu, const Float gamma ){
        
        Float y = 0;
        const UINT N = x.getSize();
        for(UINT i=0; i<N; i++){
            y += grt_sqr(x[i]-mu[i]);
        }
        return exp( gamma * y );
    }
    
    Float euclideanDist( const VectorFloat &x, const VectorFloat &y ){
        
        Float z = 0;
        const UINT N = x.getSize();
        for(UINT i=0; i<N; i++){
            z += grt_sqr(x[i]-y[i]);
        }
        return sqrt( z );
        
    }
    
protected:

    VectorFloat mean;
    
};

GRT_END_NAMESPACE

#endif //GRT_MEAN_SHIFT_HEADER
