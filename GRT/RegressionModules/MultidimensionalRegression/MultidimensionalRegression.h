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

#ifndef GRT_MULTIDIMENSIONAL_REGRESSION_HEADER
#define GRT_MULTIDIMENSIONAL_REGRESSION_HEADER

#include "../../CoreModules/Regressifier.h"
#include "../LinearRegression/LinearRegression.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This class implements the Multidimensional Regression meta algorithm. Multidimensional Regressionacts as a meta-algorithm for regression that allows several one-dimensional regression algorithms (such as Linear Regression), to be combined together to allow an M-dimensional signal to be mapped to an N-dimensional signal. This works by training N seperate regression algorithms (one for each dimension), each with an M-dimensional input.

 In addition to enabling one-dimensional regression algorithms (such as Linear Regression) to be used for
 mapping N-dimensional output signals, Multidimensional Regression can also be useful for multi-dimensional
 regression algorithms (such as Multi Layer Perceptrons), as it enables you to train N seperate MLP algorithms
 (one for each output signal), which might provide better mapping results than trying to train one MLP algorithm
 that can successfully map all N-dimensions at once.
 
 @example RegressionModulesExamples/MultidimensionalRegressionExample/MultidimensionalRegressionExample.cpp
 
 @remark This implementation is a wrapper for other GRT regression algorithms.
*/
class GRT_API MultidimensionalRegression : public Regressifier
{
public:
    /**
     Default Constructor

     @param useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
     */
	MultidimensionalRegression(const Regressifier &regressifier = LinearRegression(),bool useScaling=false);

    /**
    Copy Constructor
    
    @param rhs: copies the settings and model (if trained) from the rhs instance to this instance
    */
    MultidimensionalRegression(const MultidimensionalRegression &rhs);
    
    /**
     Default Destructor
     */
	virtual ~MultidimensionalRegression(void);
    
    /**
     Defines how the data from the rhs MultidimensionalRegression should be copied to this MultidimensionalRegression
     
     @param rhs: another instance of a MultidimensionalRegression
     @return returns a pointer to this instance of the MultidimensionalRegression
     */
	MultidimensionalRegression &operator=(const MultidimensionalRegression &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setRegressifier(...) method is called.  
     It clones the data from the Base Class Regressifier pointer (which should be pointing to an Multidimensional Regressioninstance) into this instance
     
     @param regressifier: a pointer to the Regressifier Base Class, this should be pointing to another Multidimensional Regression instance
     @return returns true if the clone was successfull, false otherwise
    */
	virtual bool deepCopyFrom(const Regressifier *regressifier);
    
    /**
     This trains the Multidimensional Regression model, using the labelled regression data.
     This overrides the train function in the ML base class.
     
     @param trainingData: the training data that will be used to train the regression model
     @return returns true if the Multidimensional Regression model was trained, false otherwise
    */
    virtual bool train_(RegressionData &trainingData);
    
    /**
     This performs the regression by mapping the inputVector using the current Multidimensional Regression model.
     This overrides the predict function in the ML base class.
     
     @param inputVector: the input Vector to classify
     @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
     This saves the trained Multidimensional Regression model to a file.
     This overrides the save function in the ML base class.
     
     @param file: a reference to the file the Multidimensional Regression model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool save( std::fstream &file ) const;
    
    /**
     This loads a trained Multidimensional Regression model from a file.
     This overrides the load function in the ML base class.
     
     @param file: a reference to the file the Logistic Regression model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool load( std::fstream &file );
    
    /**
     This returns true if the regression module has been set. The regression module is the regression algorithm
     that will be used when the user attempts to train N regression models.
     
     A regression module must be set before a regression model can be trained.
     
     @return returns true if the regression module has been set, false otherwise
     */
    bool getIsRegressionModuleSet() const;
    
    /**
     This returns a pointer to the regression module.  If no regression module has been set, then this function 
     will return NULL.
     
     @return returns a pointer to the regression module if it has been set, or NULL otherwise
     */
    Regressifier *getRegressifier() const;
    
    /**
     Sets the regression module that will be used the next time a user attempts to train a regression module.
     
     @param regressifier: a reference to a regression module that will be copied and used the next time the user attempts to train a regression module
     @return returns true if the regresion module was set successfully, false otherwise
     */
    bool setRegressionModule( const Regressifier &regressifier );

    /**
    Gets a string that represents the MDRegression class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;

protected:
    bool deepCopyRegressionModules( Vector< Regressifier* > &newModules ) const;
    bool deleteAll();
	bool deleteRegressionModules();
    bool loadLegacyModelFromFile( std::fstream &file );
	
    Regressifier *regressifier;
	Vector< Regressifier* > regressionModules;

private:
    static RegisterRegressifierModule< MultidimensionalRegression > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_MULTIDIMENSIONAL_REGRESSION_HEADER

