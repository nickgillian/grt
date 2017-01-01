
/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a basic Linear Least Squares algorithm.
 
 @remark This implementation is based on TODO
 
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

#ifndef GRT_LINEAR_LEAST_SQUARES_HEADER
#define GRT_LINEAR_LEAST_SQUARES_HEADER

#include "../../CoreModules/MLBase.h"

GRT_BEGIN_NAMESPACE

class LinearLeastSquares : public MLBase{
public:
    LinearLeastSquares() : MLBase("LinearLeastSquares") {        
        m = 0;
        b = 0;
        r = 0;
    }
    
    virtual ~LinearLeastSquares(){
        
    }

    /**
    Clears any previous results
    @return returns true if the model was cleared, false otherwise
    */
    virtual bool clear() override {
        m = 0;
        b = 0;
        r = 0;
        return MLBase::clear();
    }
    
    /**
    This is the main solver function for Linear Least Squares.
    @param x: a vector containing the x input data
    @param y: a vector containing the y input data, must have the same size as x
    @return returns true if the algorithm converged, false otherwise
    */
    bool solve( const VectorFloat &x, const VectorFloat &y ){

        //Reset any previous results
        clear();
        
        if( x.getSize() == 0 && y.getSize() == 0 ){
            warningLog << __GRT_LOG__ << " Failed to compute solution, input vectors are empty!" << std::endl;
            return false;
        }
        
        if( x.getSize() != y.getSize() ){
            warningLog << __GRT_LOG__ << " Failed to compute solution, input vectors do not have the same size!" << std::endl;
            return false;
        }
        
        const unsigned int N = x.getSize();
        Float sumx = 0.0;    //Stores the sum of x
        Float sumx2 = 0.0;   //Stores the sum of x^2
        Float sumxy = 0.0;   //Stores the sum of x * y
        Float sumy = 0.0;    //Stores the sum of y
        Float sumy2 = 0.0;   //Stores the sum of y**2
        Float denom = 0;
        
        m = 0;
        b = 0;
        r = 0;
        
        for(unsigned int i=0; i<N; i++){
            sumx  += x[i];
            sumx2 += SQR(x[i]);
            sumxy += x[i] * y[i];
            sumy  += y[i];
            sumy2 += SQR(y[i]);
        }
        
        denom = (N * sumx2 - SQR(sumx));
        if (denom == 0) {
            // singular matrix. can't solve the problem.
            warningLog << __GRT_LOG__ << " Failed to compute solution, singular matrix detected!" << std::endl;
            return false;
        }
        
        m = (N*sumxy  -  sumx*sumy) / denom;
        b = (sumy*sumx2  -  sumx*sumxy) / denom;
        
        //compute correlation coeff
        Float norm = 1.0/N;
        sumx *= norm;
        sumy *= norm;
        sumxy *= norm;
        sumx2 *= norm;
        sumy2 *= norm;
        r = (sumxy - sumx*sumy) / sqrt( (sumx2 - SQR(sumx)) * (sumy2 - SQR(sumy)) );

        trained = true;
        converged = true;
        
        return true;
    }

    /**
    Get the slope
    @return returns the slope
    */
    Float getM() const { return m; }

    /**
    Get the bias
    @return returns the bias
    */
    Float getB() const { return b; }

    /**
    Get the correlation coefficient, this indicates the goodness of fit between the data and the linear model
    @return returns the correlation coefficient
    */
    Float getR() const { return r; }
    
protected:

    Float m;
    Float b;
    Float r;
    
};

GRT_END_NAMESPACE

#endif //GRT_MEAN_SHIFT_HEADER
