/*
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

#define GRT_DLL_EXPORTS
#include "LogisticRegression.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string LogisticRegression::id = "LogisticRegression";
std::string LogisticRegression::getId() { return LogisticRegression::id; }

//Register the LogisticRegression module with the Classifier base class
RegisterRegressifierModule< LogisticRegression >  LogisticRegression::registerModule( LogisticRegression::getId() );

LogisticRegression::LogisticRegression(const bool useScaling) : Regressifier( LogisticRegression::getId() )
{
    this->useScaling = useScaling;
    minChange = 1.0e-5;
    maxNumEpochs = 500;
    learningRate = 0.01;
}

LogisticRegression::LogisticRegression(const LogisticRegression &rhs) : Regressifier( LogisticRegression::getId() )
{
    *this = rhs;
}

LogisticRegression::~LogisticRegression(void)
{
}

LogisticRegression& LogisticRegression::operator=(const LogisticRegression &rhs){
    if( this != &rhs ){
        this->w0 = rhs.w0;
        this->w = rhs.w;
        
        //Copy the base variables
        copyBaseVariables( (Regressifier*)&rhs );
    }
    return *this;
}

bool LogisticRegression::deepCopyFrom(const Regressifier *regressifier){
    
    if( regressifier == NULL ) return false;
    
    if( this->getId() == regressifier->getId() ){
        const LogisticRegression *ptr = dynamic_cast<const LogisticRegression*>(regressifier);
        
        this->w0 = ptr->w0;
        this->w = ptr->w;
        
        //Copy the base variables
        return copyBaseVariables( regressifier );
    }
    return false;
}

bool LogisticRegression::train_(RegressionData &trainingData){
    
    //Create a validation dataset, if needed
    RegressionData validationData;
    if( useValidationSet ){
        validationData = trainingData.split( 100 - validationSetSize );
    }
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumInputDimensions();
    const unsigned int K = trainingData.getNumTargetDimensions();
    trained = false;
    trainingResults.clear();
    
    if( M == 0 ){
        errorLog << "train_(RegressionData trainingData) - Training data has zero samples!" << std::endl;
        return false;
    }
    
    if( K == 0 ){
        errorLog << "train_(RegressionData trainingData) - The number of target dimensions is not 1!" << std::endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = 1; //Logistic Regression will have 1 output
    inputVectorRanges.clear();
    targetVectorRanges.clear();
   
    //Scale the training and validation data, if needed
    if( useScaling ){
        //Find the ranges for the input data
        inputVectorRanges = trainingData.getInputRanges();
        
        //Find the ranges for the target data
        targetVectorRanges = trainingData.getTargetRanges();
        
        //Scale the training data
        trainingData.scale(inputVectorRanges,targetVectorRanges,0.0,1.0);
    
        //Scale the validation data
        if( useValidationSet ){
          validationData.scale(inputVectorRanges,targetVectorRanges,0.0,1.0);
        }
    }
    
    //Reset the weights
    Random rand;
    w0 = rand.getRandomNumberUniform(-0.1,0.1);
    w.resize(N);
    for(UINT j=0; j<N; j++){
        w[j] = rand.getRandomNumberUniform(-0.1,0.1);
    }
    
    Float error = 0;
    Float lastError = 0;
    Float delta = 0;
    UINT epoch = 0;
    bool keepTraining = true;
    Random random;
    Vector< UINT > randomTrainingOrder(M);
    TrainingResult result;
    trainingResults.reserve(M);
    
    //In most cases, the training data is grouped into classes (100 samples for class 1, followed by 100 samples for class 2, etc.)
    //This can cause a problem for stochastic gradient descent algorithm. To avoid this issue, we randomly shuffle the order of the
    //training samples. This random order is then used at each epoch.
    for(UINT i=0; i<M; i++){
        randomTrainingOrder[i] = i;
    }
    std::random_shuffle(randomTrainingOrder.begin(), randomTrainingOrder.end());
    
    //Run the main stochastic gradient descent training algorithm
    while( keepTraining ){

      rmsTrainingError = 0.0;
        
        //Run one epoch of training using stochastic gradient descent
        for(UINT m=0; m<M; m++){
            
            //Select the random sample
            UINT i = randomTrainingOrder[m];
            
            //Compute the error, given the current weights
            VectorFloat x = trainingData[i].getInputVector();
            VectorFloat y = trainingData[i].getTargetVector();
            Float h = w0;
            for(UINT j=0; j<N; j++){
                h += x[j] * w[j];
            }
            error = y[0] - sigmoid( h );
            rmsTrainingError += SQR(error);
            
            //Update the weights
            for(UINT j=0; j<N; j++){
                w[j] += learningRate * error * x[j];
            }
            w0 += learningRate * error;
        }
        
        //Compute the error on the validation set if needed
        Float rmsValidationError = 0;
        if( useValidationSet ){
          for(UINT i=0; i<validationData.getNumSamples(); i++){
            //Compute the error, given the current weights
            const VectorFloat &x = validationData[i].getInputVector();
            const VectorFloat &y = validationData[i].getTargetVector();
            Float h = w0;
            for(UINT j=0; j<N; j++){
                h += x[j] * w[j];
            }
            error = y[0] - sigmoid( h );
            rmsValidationError += SQR(error);
          }
          rmsValidationError = sqrt( rmsValidationError / Float(validationData.getNumSamples()) );
        }

        //Compute the error
        rmsTrainingError = sqrt( rmsTrainingError / Float(M) );
        delta = epoch > 0 ? fabs( rmsTrainingError-lastError ) : rmsTrainingError;
        lastError = rmsTrainingError;

        //Check to see if we should stop
        if( delta <= minChange ){
            keepTraining = false;
        }
        
        if( ++epoch >= maxNumEpochs ){
            keepTraining = false;
        }
        
        if( grt_isinf( rmsTrainingError ) || grt_isnan( rmsTrainingError ) ){
            errorLog << "train_(RegressionData &trainingData) - Training failed! RMS error is NAN. If scaling is not enabled then you should try to scale your data and see if this solves the issue." << std::endl;
            return false;
        }
        
        //Store the training results
        result.setRegressionResult(epoch,rmsTrainingError,rmsValidationError,this);
        trainingResults.push_back( result );
        
        //Notify any observers of the new result
        trainingResultsObserverManager.notifyObservers( result );
        
        trainingLog << "Epoch: " << epoch << " RMS Training Error: " << rmsTrainingError << " Delta: " << delta << " RMS Validation Error: " << rmsValidationError << std::endl;
    }
    
    //Flag that the algorithm has been trained
    regressionData.resize(1,0);
    trained = true;
    return trained;
}

bool LogisticRegression::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - Model Not Trained!" << std::endl;
        return false;
    }
    
    if( !trained ) return false;
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &inputVector) - The size of the input Vector (" << inputVector.getSize() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = grt_scale(inputVector[n], inputVectorRanges[n].minValue, inputVectorRanges[n].maxValue, 0.0, 1.0);
        }
    }
    
    regressionData[0] =  w0;
    for(UINT j=0; j<numInputDimensions; j++){
        regressionData[0] += inputVector[j] * w[j];
    }
    Float sum = regressionData[0];
    regressionData[0] = sigmoid( regressionData[0] );
    if( useScaling ){
        for(UINT n=0; n<numOutputDimensions; n++){
            regressionData[n] = grt_scale(regressionData[n], 0.0, 1.0, targetVectorRanges[n].minValue, targetVectorRanges[n].maxValue);
        }
    }
    
    return true;
}

bool LogisticRegression::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file<<"GRT_LOGISTIC_REGRESSION_MODEL_FILE_V2.0\n";
    
    //Write the regressifier settings to the file
    if( !Regressifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save Regressifier base settings to file!" << std::endl;
        return false;
    }
    
    if( trained ){
        file << "Weights: ";
        file << w0;
        for(UINT j=0; j<numInputDimensions; j++){
            file << " " << w[j];
        }
        file << std::endl;
    }
    
    return true;
}

bool LogisticRegression::load( std::fstream &file ){
    
    trained = false;
    numInputDimensions = 0;
    w0 = 0;
    w.clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Find the file type header
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_LOGISTIC_REGRESSION_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    if( word != "GRT_LOGISTIC_REGRESSION_MODEL_FILE_V2.0" ){
        errorLog << "load( fstream &file ) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the regressifier settings from the file
    if( !Regressifier::loadBaseSettingsFromFile(file) ){
        errorLog <<"load( fstream &file ) - Failed to save Regressifier base settings to file!" << std::endl;
        return false;
    }
    
    if( trained ){
        
        //Resize the weights
        w.resize(numInputDimensions);
        
        //Load the weights
        file >> word;
        if(word != "Weights:"){
            errorLog << "load( fstream &file ) - Could not find the Weights!" << std::endl;
            return false;
        }
        
        file >> w0;
        for(UINT j=0; j<numInputDimensions; j++){
            file >> w[j];
            
        }
    }
    
    return true;
}

UINT LogisticRegression::getMaxNumIterations() const{
    return getMaxNumEpochs();
}

bool LogisticRegression::setMaxNumIterations(const UINT maxNumIterations){
    return setMaxNumEpochs( maxNumIterations );
}

Float LogisticRegression::sigmoid(const Float x) const{
    return 1.0 / (1 + exp(-x));
}

bool LogisticRegression::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadLegacyModelFromFile( fstream &file ) - Could not find NumFeatures!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumOutputDimensions:"){
        errorLog << "loadLegacyModelFromFile( fstream &file ) - Could not find NumOutputDimensions!" << std::endl;
        return false;
    }
    file >> numOutputDimensions;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadLegacyModelFromFile( fstream &file ) - Could not find UseScaling!" << std::endl;
        return false;
    }
    file >> useScaling;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        inputVectorRanges.resize(numInputDimensions);
        targetVectorRanges.resize(numOutputDimensions);
        
        //Load the ranges
        file >> word;
        if(word != "InputVectorRanges:"){
            file.close();
            errorLog << "loadLegacyModelFromFile( fstream &file ) - Failed to find InputVectorRanges!" << std::endl;
            return false;
        }
        for(UINT j=0; j<inputVectorRanges.getSize(); j++){
            file >> inputVectorRanges[j].minValue;
            file >> inputVectorRanges[j].maxValue;
        }
        
        file >> word;
        if(word != "OutputVectorRanges:"){
            file.close();
            errorLog << "loadLegacyModelFromFile( fstream &file ) - Failed to find OutputVectorRanges!" << std::endl;
            return false;
        }
        for(UINT j=0; j<targetVectorRanges.getSize(); j++){
            file >> targetVectorRanges[j].minValue;
            file >> targetVectorRanges[j].maxValue;
        }
    }
    
    //Resize the weights
    w.resize(numInputDimensions);
    
    //Load the weights
    file >> word;
    if(word != "Weights:"){
        errorLog << "loadLegacyModelFromFile( fstream &file ) - Could not find the Weights!" << std::endl;
        return false;
    }
    
    file >> w0;
    for(UINT j=0; j<numInputDimensions; j++){
        file >> w[j];
        
    }
    
    //Resize the regression data Vector
    regressionData.resize(1,0);
    
    //Flag that the model has been trained
    trained = true;
    
    return true;
}

GRT_END_NAMESPACE
