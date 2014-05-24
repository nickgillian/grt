/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief The TestResult class provides a data structure for storing the results of a classification or regression test.
 */

#ifndef GRT_TEST_RESULT_HEADER
#define GRT_TEST_RESULT_HEADER

#include "GRTTypedefs.h"
#include "MatrixDouble.h"

namespace GRT {

class TestResult{
public:
    /**
     Default Constructor.
     
     Initializes the TestResult instance.
     */
    TestResult(){
    }
    
    /**
     Copy Constructor.
     
     Initializes this instance by copying the data from the rhs instance
     
     @param const TestResult &rhs: another instance of the TestResult class
     */
    TestResult(const TestResult &rhs){
        *this = rhs;
    }
    
    /**
     Default Destructor.
     */
    ~TestResult(){
        
    }

    TestResult& operator=(const TestResult &rhs){
        if( this != &rhs ){
            this->numTrainingSamples = rhs.numTrainingSamples;
            this->numTestSamples = rhs.numTestSamples;
            this->accuracy = rhs.accuracy;
            this->rmsError = rhs.rmsError;
            this->totalSquaredError = rhs.totalSquaredError;
            this->trainingTime = rhs.trainingTime;
            this->testTime = rhs.testTime;
            this->rejectionPrecision = rhs.rejectionPrecision;
            this->rejectionRecall = rhs.rejectionRecall;
            this->precision = rhs.precision;
            this->recall = rhs.recall;
            this->fMeasure = rhs.fMeasure;
            this->confusionMatrix = rhs.confusionMatrix;
        }
        return *this;
    }
    
    unsigned int numTrainingSamples;
    unsigned int numTestSamples;
    double accuracy;
    double rmsError;
    double totalSquaredError;
    double trainingTime;
    double testTime;
    double rejectionPrecision;
    double rejectionRecall;
    VectorDouble precision;
    VectorDouble recall;
    VectorDouble fMeasure;
    MatrixDouble confusionMatrix;

};

}//End of namespace GRT

#endif //GRT_TEST_RESULT_HEADER
