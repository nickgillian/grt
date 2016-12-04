/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 
 @brief This file contains the Metrics class, it can be used to compute common metrics such as accuracy, rms error, etc..
 */

#ifndef GRT_METRICS_HEADER
#define GRT_METRICS_HEADER

#include "GRTTypedefs.h"

GRT_BEGIN_NAMESPACE

//Forward declaration
class Classifier;
class ClassificationData;

class GRT_API Metrics{
public:
	/**
	Default constructor.
	*/
    Metrics();

	/**
	Default destructor.
	*/
    ~Metrics();

    /**
    Computes the accuracy of the model given the input dataset. The accuracy results will be stored in the accuracy parameter and will be in the range of [0., 100.0].
    @param model: the trained model you want to test the accuracy using the input dataset
    @param data: the dataset that will be used to test the model
    @param accuracy: the variable to which the accuracy of the model will be stored 
    @return returns true if the accuracy was computed, false otherwise
    */
    static bool computeAccuracy( Classifier &model, const ClassificationData &data, Float &accuracy );
};
    
GRT_END_NAMESPACE

#endif // GRT_UTIL_HEADER
