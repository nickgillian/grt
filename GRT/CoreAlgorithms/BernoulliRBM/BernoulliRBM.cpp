
#define GRT_DLL_EXPORTS
#include "BernoulliRBM.h"

GRT_BEGIN_NAMESPACE

BernoulliRBM::BernoulliRBM(const UINT numHiddenUnits,const UINT maxNumEpochs,const Float learningRate,const Float learningRateUpdate,const Float momentum,const bool useScaling,const bool randomiseTrainingOrder) : MLBase("BernoulliRBM")
{
    
    this->numHiddenUnits = numHiddenUnits;
    this->maxNumEpochs = maxNumEpochs;
    this->learningRate = learningRate;
    this->learningRateUpdate = learningRateUpdate;
    this->momentum = momentum;
    this->useScaling = useScaling;
    this->randomiseTrainingOrder = randomiseTrainingOrder;
    randomizeWeightsForTraining = true;
    batchSize = 100;
    batchStepSize = 1;
    minNumEpochs = 1;
    minChange = 1.0e-5;
}

BernoulliRBM::~BernoulliRBM(){
    
}

bool BernoulliRBM::predict_(VectorFloat &inputData){
    
    if( !predict_(inputData,outputData) ){
        return false;
    }
    
    return true;
}

bool BernoulliRBM::predict_(VectorFloat &inputData,VectorFloat &outputData){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputData,VectorFloat &outputData) - Failed to run prediction - the model has not been trained." << std::endl;
        return false;
    }
    
    if( inputData.size() != numVisibleUnits ){
        errorLog << "predict_(VectorFloat &inputData,VectorFloat &outputData) - Failed to run prediction - the input data size (" << inputData.size() << ")";
        errorLog << " does not match the number of visible units (" << numVisibleUnits << "). " << std::endl;
        return false;
    }
    
    if( outputData.size() != numHiddenUnits ){
        outputData.resize( numHiddenUnits );
    }
    
    //Scale the data if needed
    if( useScaling ){
        for(UINT i=0; i<numVisibleUnits; i++){
            inputData[i] = grt_scale(inputData[i],ranges[i].minValue,ranges[i].maxValue,0.0,1.0);
        }
    }
    
    //Propagate the data up through the RBM
    Float x = 0.0;
    for(UINT i=0; i<numHiddenUnits; i++){
        x = 0.0;
        for(UINT j=0; j<numVisibleUnits; j++) {
            x += weightsMatrix[i][j] * inputData[j];
        }
        outputData[i] = grt_sigmoid( x + hiddenLayerBias[i] );
    }
    
    return true;
}

bool BernoulliRBM::predict_(const MatrixFloat &inputData,MatrixFloat &outputData,const UINT rowIndex){
    
    if( !trained ){
        errorLog << "predict_(const MatrixFloat &inputData,MatrixFloat &outputData,const UINT rowIndex) - Failed to run prediction - the model has not been trained." << std::endl;
        return false;
    }
    
    if( inputData.getNumCols() != numVisibleUnits ){
        errorLog << "predict_(const MatrixFloat &inputData,MatrixFloat &outputData,const UINT rowIndex) -";
        errorLog << " Failed to run prediction - the number of columns in the input matrix (" << inputData.getNumCols() << ")";
        errorLog << " does not match the number of visible units (" << numVisibleUnits << ")." << std::endl;
        return false;
    }
    
    if( outputData.getNumCols() != numHiddenUnits ){
        errorLog << "predict_(const MatrixFloat &inputData,MatrixFloat &outputData,const UINT rowIndex) -";
        errorLog << " Failed to run prediction - the number of columns in the output matrix (" << outputData.getNumCols() << ")";
        errorLog << " does not match the number of hidden units (" << numHiddenUnits << ")." << std::endl;
        return false;
    }
    
    //Propagate the data up through the RBM
    Float x = 0.0;
    for(UINT j=0; j<numHiddenUnits; j++){
        x = 0;
        for(UINT i=0; i<numVisibleUnits; i++) {
            x += weightsMatrix[j][i] * inputData[rowIndex][i];
        }
        outputData[rowIndex][j] = grt_sigmoid( x + hiddenLayerBias[j] ); //This gives P( h_j = 1 | input )
    }
    
    return true;
}

bool BernoulliRBM::train_(MatrixFloat &data){
    
    const UINT numTrainingSamples = data.getNumRows();
    numInputDimensions = data.getNumCols();
    numOutputDimensions = numHiddenUnits;
    numVisibleUnits = numInputDimensions;
    
    trainingLog << "NumInputDimensions: " << numInputDimensions << std::endl;
    trainingLog << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    
    if( randomizeWeightsForTraining ){
        
        //Init the weights matrix
        weightsMatrix.resize(numHiddenUnits, numVisibleUnits);
        
        Float a = 1.0 / numVisibleUnits;
        for(UINT i=0; i<numHiddenUnits; i++) {
            for(UINT j=0; j<numVisibleUnits; j++) {
                weightsMatrix[i][j] = rand.getRandomNumberUniform(-a, a);
            }
        }
        
        //Init the bias units
        visibleLayerBias.resize( numVisibleUnits );
        hiddenLayerBias.resize( numHiddenUnits );
        std::fill(visibleLayerBias.begin(),visibleLayerBias.end(),0);
        std::fill(hiddenLayerBias.begin(),hiddenLayerBias.end(),0);
        
    }else{
        if( weightsMatrix.getNumRows() != numHiddenUnits ){
            errorLog << "train_(MatrixFloat &data) - Weights matrix row size does not match the number of hidden units!" << std::endl;
            return false;
        }
        if( weightsMatrix.getNumCols() != numVisibleUnits ){
            errorLog << "train_(MatrixFloat &data) - Weights matrix row size does not match the number of visible units!" << std::endl;
            return false;
        }
        if( visibleLayerBias.size() != numVisibleUnits ){
            errorLog << "train_(MatrixFloat &data) - Visible layer bias size does not match the number of visible units!" << std::endl;
            return false;
        }
        if( hiddenLayerBias.size() != numHiddenUnits ){
            errorLog << "train_(MatrixFloat &data) - Hidden layer bias size does not match the number of hidden units!" << std::endl;
            return false;
        }
    }
    
    //Flag the model has been trained encase the user wants to save the model during a training iteration using an observer
    trained = true;
    
    //Make sure the data is scaled between [0 1]
    ranges = data.getRanges();
    if( useScaling ){
        for(UINT i=0; i<numTrainingSamples; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                data[i][j] = grt_scale(data[i][j], ranges[j].minValue, ranges[j].maxValue, 0.0, 1.0);
            }
        }
    }
    
    
    const UINT numBatches = static_cast<UINT>( ceil( Float(numTrainingSamples)/batchSize ) );
    
    //Setup the batch indexs
    Vector< BatchIndexs > batchIndexs( numBatches );
    UINT startIndex = 0;
    for(UINT i=0; i<numBatches; i++){
        batchIndexs[i].startIndex = startIndex;
        batchIndexs[i].endIndex = startIndex + batchSize;
        
        //Make sure the last batch end index is not larger than the number of training examples
        if( batchIndexs[i].endIndex >= numTrainingSamples ){
            batchIndexs[i].endIndex = numTrainingSamples;
        }
        
        //Get the batch size
        batchIndexs[i].batchSize = batchIndexs[i].endIndex - batchIndexs[i].startIndex;
        
        //Set the start index for the next batch
        startIndex = batchIndexs[i].endIndex;
    }
    
    Timer timer;
    UINT i,j,n,epoch,noChangeCounter = 0;
    Float startTime = 0;
    Float alpha = learningRate;
    Float error = 0;
    Float err = 0;
    Float delta = 0;
    Float lastError = 0;
    Vector< UINT > indexList(numTrainingSamples);
    TrainingResult trainingResult;
    MatrixFloat wT( numVisibleUnits, numHiddenUnits );       //Stores a transposed copy of the weights vector
    MatrixFloat vW( numHiddenUnits, numVisibleUnits );       //Stores the weight velocity updates
    MatrixFloat tmpW( numHiddenUnits, numVisibleUnits );     //Stores the weight values that will be used to update the main weights matrix at each batch update
    MatrixFloat v1( batchSize, numVisibleUnits );            //Stores the real batch data during a batch update
    MatrixFloat v2( batchSize, numVisibleUnits );            //Stores the sampled batch data during a batch update
    MatrixFloat h1( batchSize, numHiddenUnits );             //Stores the hidden states given v1 and the current weightsMatrix
    MatrixFloat h2( batchSize, numHiddenUnits );             //Stores the sampled hidden states given v2 and the current weightsMatrix
    MatrixFloat c1( numHiddenUnits, numVisibleUnits );       //Stores h1' * v1
    MatrixFloat c2( numHiddenUnits, numVisibleUnits );       //Stores h2' * v2
    MatrixFloat vDiff( batchSize, numVisibleUnits );         //Stores the difference between v1-v2
    MatrixFloat hDiff( batchSize, numVisibleUnits );         //Stores the difference between h1-h2
    MatrixFloat cDiff( numHiddenUnits, numVisibleUnits );    //Stores the difference between c1-c2
    VectorFloat vDiffSum( numVisibleUnits );                 //Stores the column sum of vDiff
    VectorFloat hDiffSum( numHiddenUnits );                  //Stores the column sum of hDiff
    VectorFloat visibleLayerBiasVelocity( numVisibleUnits ); //Stores the velocity update of the visibleLayerBias
    VectorFloat hiddenLayerBiasVelocity( numHiddenUnits );   //Stores the velocity update of the hiddenLayerBias
    
    //Set all the velocity weights to zero
    vW.setAllValues( 0 );
    std::fill(visibleLayerBiasVelocity.begin(),visibleLayerBiasVelocity.end(),0);
    std::fill(hiddenLayerBiasVelocity.begin(),hiddenLayerBiasVelocity.end(),0);
    
    //Randomize the order that the training samples will be used in
    for(UINT i=0; i<numTrainingSamples; i++) indexList[i] = i;
    if( randomiseTrainingOrder ){
        std::random_shuffle(indexList.begin(), indexList.end());
    }
    
    //Start the main training loop
    timer.start();
    for(epoch=0; epoch<maxNumEpochs; epoch++) {
        startTime = timer.getMilliSeconds();
        error = 0;
        
        //Randomize the batch order
        std::random_shuffle(batchIndexs.begin(),batchIndexs.end());
        
        //Run each of the batch updates
        for(UINT k=0; k<numBatches; k+=batchStepSize){
            
            //Resize the data matrices, the matrices will only be resized if the rows cols are different
            v1.resize( batchIndexs[k].batchSize, numVisibleUnits );
            h1.resize( batchIndexs[k].batchSize, numHiddenUnits );
            v2.resize( batchIndexs[k].batchSize, numVisibleUnits );
            h2.resize( batchIndexs[k].batchSize, numHiddenUnits );
            
            //Setup the data pointers, using data pointers saves a few ms on large matrix updates
            Float **w_p = weightsMatrix.getDataPointer();
            Float **wT_p = wT.getDataPointer();
            Float **vW_p = vW.getDataPointer();
            Float **data_p = data.getDataPointer();
            Float **v1_p = v1.getDataPointer();
            Float **v2_p = v2.getDataPointer();
            Float **h1_p = h1.getDataPointer();
            Float **h2_p = h2.getDataPointer();
            Float *vlb_p = &visibleLayerBias[0];
            Float *hlb_p = &hiddenLayerBias[0];
            
            //Get the batch data
            UINT index = 0;
            for(i=batchIndexs[k].startIndex; i<batchIndexs[k].endIndex; i++){
                for(j=0; j<numVisibleUnits; j++){
                    v1_p[index][j] = data_p[ indexList[i] ][j];
                }
                index++;
            }
            
            //Copy a transposed version of the weights matrix, this is used to compute h1 and h2
            for(i=0; i<numHiddenUnits; i++)
            for(j=0; j<numVisibleUnits; j++)
            wT_p[j][i] = w_p[i][j];
            
            //Compute h1
            h1.multiple(v1, wT);
            for(n=0; n<batchIndexs[k].batchSize; n++){
                for(i=0; i<numHiddenUnits; i++){
                    h1_p[n][i] = sigmoidRandom( h1_p[n][i] + hlb_p[i] );
                }
            }
            
            //Compute v2
            v2.multiple(h1, weightsMatrix);
            for(n=0; n<batchIndexs[k].batchSize; n++){
                for(i=0; i<numVisibleUnits; i++){
                    v2_p[n][i] = sigmoidRandom( v2_p[n][i] + vlb_p[i] );
                }
            }
            
            //Compute h2
            h2.multiple(v2,wT);
            for(n=0; n<batchIndexs[k].batchSize; n++){
                for(i=0; i<numHiddenUnits; i++){
                    h2_p[n][i] = grt_sigmoid( h2_p[n][i] + hlb_p[i] );
                }
            }
            
            //Compute c1, c2 and the difference between v1-v2
            c1.multiple(h1,v1,true);
            c2.multiple(h2,v2,true);
            vDiff.subtract(v1, v2);
            
            //Compute the sum of vdiff
            for(j=0; j<numVisibleUnits; j++){
                vDiffSum[j] = 0;
                for(i=0; i<batchIndexs[k].batchSize; i++){
                    vDiffSum[j] += vDiff[i][j];
                }
            }
            
            //Compute the difference between h1 and h2
            hDiff.subtract(h1, h2);
            for(j=0; j<numHiddenUnits; j++){
                hDiffSum[j] = 0;
                for(i=0; i<batchIndexs[k].batchSize; i++){
                    hDiffSum[j] += hDiff[i][j];
                }
            }
            
            //Compute the difference between c1 and c2
            cDiff.subtract(c1,c2);
            
            //Update the weight velocities
            for(i=0; i<numHiddenUnits; i++){
                for(j=0; j<numVisibleUnits; j++){
                    vW_p[i][j] = ((momentum * vW_p[i][j]) + (alpha * cDiff[i][j])) / batchIndexs[k].batchSize;
                }
            }
            for(i=0; i<numVisibleUnits; i++){
                visibleLayerBiasVelocity[i] = ((momentum * visibleLayerBiasVelocity[i]) + (alpha * vDiffSum[i])) / batchIndexs[k].batchSize;
            }
            for(i=0; i<numHiddenUnits; i++){
                hiddenLayerBiasVelocity[i] = ((momentum * hiddenLayerBiasVelocity[i]) + (alpha * hDiffSum[i])) / batchIndexs[k].batchSize;
            }
            
            //Update the weights
            weightsMatrix.add( vW );
            
            //Update the bias for the visible layer
            for(i=0; i<numVisibleUnits; i++){
                visibleLayerBias[i] += visibleLayerBiasVelocity[i];
            }
            
            //Update the bias for the visible layer
            for(i=0; i<numHiddenUnits; i++){
                hiddenLayerBias[i] += hiddenLayerBiasVelocity[i];
            }
            
            //Compute the reconstruction error
            err = 0;
            for(i=0; i<batchIndexs[k].batchSize; i++){
                for(j=0; j<numVisibleUnits; j++){
                    err += SQR( v1[i][j] - v2[i][j] );
                }
            }
            
            error += err / batchIndexs[k].batchSize;
        }
        error /= numBatches;
        delta = lastError - error;
        lastError = error;
        
        trainingLog << "Epoch: " << epoch+1 << "/" << maxNumEpochs;
        trainingLog << " Epoch time: " << (timer.getMilliSeconds()-startTime)/1000.0 << " seconds";
        trainingLog << " Learning rate: " << alpha;
        trainingLog << " Momentum: " << momentum;
        trainingLog << " Average reconstruction error: " << error;
        trainingLog << " Delta: " << delta << std::endl;
        
        //Update the learning rate
        alpha *= learningRateUpdate;
        
        trainingResult.setClassificationResult(epoch, error, this);
        trainingResults.push_back(trainingResult);
        trainingResultsObserverManager.notifyObservers( trainingResult );
        
        //Check for convergance
        if( fabs(delta) < minChange ){
            if( ++noChangeCounter >= minNumEpochs ){
                trainingLog << "Stopping training. MinChange limit reached!" << std::endl;
                break;
            }
        }else noChangeCounter = 0;
        
    }
    trainingLog << "Training complete after " << epoch << " epochs. Total training time: " << timer.getMilliSeconds()/1000.0 << " seconds" << std::endl;
    
    trained = true;
    
    return true;
}

bool BernoulliRBM::reset(){
    
    //Reset the base class
    MLBase::reset();
    
    return true;
}

bool BernoulliRBM::clear(){
    
    //Clear the base class
    MLBase::clear();
    
    weightsMatrix.clear();
    weightsMatrix.clear();
    visibleLayerBias.clear();
    hiddenLayerBias.clear();
    ph_mean.clear();
    ph_sample.clear();
    nv_means.clear();
    nv_samples.clear();
    nh_means.clear();
    nh_samples.clear();
    outputData.clear();
    ranges.clear();
    
    randomizeWeightsForTraining = true;
    
    return true;
}

bool BernoulliRBM::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file<<"GRT_BERNOULLI_RBM_MODEL_FILE_V1.1\n";
    
    if( !saveBaseSettingsToFile( file ) ){
        errorLog <<"save(fstream &file) - Failed to save base settings to file!" << std::endl;
        return false;
    }
    
    file << "NumVisibleUnits: " << numVisibleUnits << std::endl;
    file << "NumHiddenUnits: " << numHiddenUnits << std::endl;
    file << "BatchSize: " << batchSize << std::endl;
    file << "BatchStepSize: " << batchStepSize << std::endl;
    file << "LearningRate: " << learningRate << std::endl;
    file << "LearningRateUpdate: " << learningRateUpdate << std::endl;
    file << "Momentum: " << momentum << std::endl;
    file << "RandomizeWeightsForTraining: " << randomizeWeightsForTraining << std::endl;
    
    file << "Ranges: \n";
    for(UINT n=0; n<ranges.size(); n++){
        file << ranges[n].minValue << "\t" << ranges[n].maxValue << std::endl;
    }
    
    //If the model has been trained then write the model
    if( trained ){
        file << "WeightsMatrix: " << std::endl;
        for(UINT i=0; i<weightsMatrix.getNumRows(); i++){
            for(UINT j=0; j<weightsMatrix.getNumCols(); j++){
                file << weightsMatrix[i][j];
                if( j < weightsMatrix.getNumCols()-1 ) file << " ";
            }
            file << std::endl;
        }
        
        file << "VisibleLayerBias: ";
        for(unsigned int i=0; i<visibleLayerBias.size(); i++){
            file << visibleLayerBias[i];
            if( i < visibleLayerBias.size()-1 ) file << " ";
        }
        file << std::endl;
        
        file << "HiddenLayerBias: ";
        for(unsigned int i=0; i<hiddenLayerBias.size(); i++){
            file << hiddenLayerBias[i];
            if( i < hiddenLayerBias.size()-1 ) file << " ";
        }
        file << std::endl;
    }
    
    return true;
}

bool BernoulliRBM::load( std::fstream &file ){
    
    if(!file.is_open())
    {
        errorLog <<"load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Read the header info
    file >> word;
    
    if( word == "GRT_BERNOULLI_RBM_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    if( word != "GRT_BERNOULLI_RBM_MODEL_FILE_V1.1" ){
        errorLog <<"load(fstream &file) - Failed to read file header!" << std::endl;
        return false;
    }
    
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog <<"load(fstream &file) - Failed to load base settings to file!" << std::endl;
        return false;
    }
    
    //Read the number of visible units
    file >> word;
    if( word != "NumVisibleUnits:" ){
        errorLog <<"load(fstream &file) - Failed to read NumVisibleUnits header!" << std::endl;
        return false;
    }
    file >> numVisibleUnits;
    
    //Read the number of hidden units
    file >> word;
    if( word != "NumHiddenUnits:" ){
        errorLog <<"load(fstream &file) - Failed to read NumHiddenUnits header!" << std::endl;
        return false;
    }
    file >> numHiddenUnits;
    
    //Read the batch size
    file >> word;
    if( word != "BatchSize:" ){
        errorLog <<"load(fstream &file) - Failed to read BatchSize header!" << std::endl;
        return false;
    }
    file >> batchSize;
    
    //Read the batch step size
    file >> word;
    if( word != "BatchStepSize:" ){
        errorLog <<"load(fstream &file) - Failed to read BatchStepSize header!" << std::endl;
        return false;
    }
    file >> batchStepSize;
    
    //Read the learning rate
    file >> word;
    if( word != "LearningRate:" ){
        errorLog <<"load(fstream &file) - Failed to read LearningRate header!" << std::endl;
        return false;
    }
    file >> learningRate;
    
    //Read the learning rate update
    file >> word;
    if( word != "LearningRateUpdate:" ){
        errorLog <<"load(fstream &file) - Failed to read LearningRateUpdate header!" << std::endl;
        return false;
    }
    file >> learningRateUpdate;
    
    //Read the momentum
    file >> word;
    if( word != "Momentum:" ){
        errorLog <<"load(fstream &file) - Failed to read Momentum header!" << std::endl;
        return false;
    }
    file >> momentum;
    
    //Read the randomizeWeightsForTraining
    file >> word;
    if( word != "RandomizeWeightsForTraining:" ){
        errorLog <<"load(fstream &file) - Failed to read RandomizeWeightsForTraining header!" << std::endl;
        return false;
    }
    file >> randomizeWeightsForTraining;
    
    //Read the ranges
    file >> word;
    if( word != "Ranges:" ){
        errorLog <<"load(fstream &file) - Failed to read Ranges header!" << std::endl;
        return false;
    }
    ranges.resize(numInputDimensions);
    for(UINT n=0; n<ranges.size(); n++){
        file >> ranges[n].minValue;
        file >> ranges[n].maxValue;
    }
    
    //If the model has been trained then load the model
    if( trained ){
        
        //Load the weights matrix
        file >> word;
        if( word != "WeightsMatrix:" ){
            errorLog <<"load(fstream &file) - Failed to read WeightsMatrix header!" << std::endl;
            return false;
        }
        weightsMatrix.resize(numHiddenUnits, numVisibleUnits);
        
        for(UINT i=0; i<weightsMatrix.getNumRows(); i++){
            for(UINT j=0; j<weightsMatrix.getNumCols(); j++){
                file >> weightsMatrix[i][j];
            }
        }
        
        //Load the VisibleLayerBias
        file >> word;
        if( word != "VisibleLayerBias:" ){
            errorLog <<"load(fstream &file) - Failed to read VisibleLayerBias header!" << std::endl;
            return false;
        }
        visibleLayerBias.resize(numVisibleUnits);
        
        for(unsigned int i=0; i<visibleLayerBias.size(); i++){
            file >> visibleLayerBias[i];
        }
        
        //Load the HiddenLayerBias
        file >> word;
        if( word != "HiddenLayerBias:" ){
            errorLog <<"load(fstream &file) - Failed to read HiddenLayerBias header!" << std::endl;
            return false;
        }
        hiddenLayerBias.resize(numHiddenUnits);
        
        for(unsigned int i=0; i<hiddenLayerBias.size(); i++){
            file >> hiddenLayerBias[i];
        }
    }
    
    return true;
}

bool BernoulliRBM::print() const{
    
    if( !trained ){
        return false;
    }
    
    std::cout << "WeightsMatrix: \n";
    for(UINT i=0; i<numVisibleUnits; i++) {
        for(UINT j=0; j<numHiddenUnits; j++) {
            std::cout << weightsMatrix[j][i] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "visible layer bias: ";
    for(UINT j=0; j<numVisibleUnits; j++) {
        std::cout << visibleLayerBias[j] << "\t";
    }
    std::cout << std::endl;
    
    std::cout << "hidden layer bias: ";
    for(UINT j=0; j<numHiddenUnits; j++) {
        std::cout << hiddenLayerBias[j] << "\t";
    }
    std::cout << std::endl;
    
    return true;
}

bool BernoulliRBM::getRandomizeWeightsForTraining() const{
    return randomizeWeightsForTraining;
}

UINT BernoulliRBM::getNumVisibleUnits() const{
    return numVisibleUnits;
}

UINT BernoulliRBM::getNumHiddenUnits() const{
    return numHiddenUnits;
}

const MatrixFloat& BernoulliRBM::getWeights() const{
    return weightsMatrix;
}

VectorFloat BernoulliRBM::getOutputData()const{
    return outputData;
}

bool BernoulliRBM::setNumHiddenUnits(const UINT numHiddenUnits){
    this->numHiddenUnits = numHiddenUnits;
    clear();
    return true;
}

bool BernoulliRBM::setMomentum(const Float momentum){
    this->momentum = momentum;
    return true;
}

bool BernoulliRBM::setLearningRateUpdate(const Float learningRateUpdate){
    this->learningRateUpdate = learningRateUpdate;
    return true;
}

bool BernoulliRBM::setRandomizeWeightsForTraining(const bool randomizeWeightsForTraining){
    this->randomizeWeightsForTraining = randomizeWeightsForTraining;
    return true;
}

bool BernoulliRBM::setBatchSize(const UINT batchSize){
    this->batchSize = batchSize;
    return true;
}

bool BernoulliRBM::setBatchStepSize(const UINT batchStepSize){
    this->batchStepSize = batchStepSize;
    return true;
}

bool BernoulliRBM::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    UINT numGibbsSteps = 0;
    
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog <<"load(fstream &file) - Failed to load base settings to file!" << std::endl;
        return false;
    }
    
    //Read the number of visible units
    file >> word;
    if( word != "NumVisibleUnits:" ){
        errorLog <<"load(fstream &file) - Failed to read NumVisibleUnits header!" << std::endl;
        return false;
    }
    file >> numVisibleUnits;
    
    //Read the number of hidden units
    file >> word;
    if( word != "NumHiddenUnits:" ){
        errorLog <<"load(fstream &file) - Failed to read NumHiddenUnits header!" << std::endl;
        return false;
    }
    file >> numHiddenUnits;
    
    //Read the number of training epochs
    file >> word;
    if( word != "NumTrainingEpochs:" ){
        errorLog <<"load(fstream &file) - Failed to read NumTrainingEpochs header!" << std::endl;
        return false;
    }
    file >> maxNumEpochs;
    
    //Read the number of gibbs steps
    file >> word;
    if( word != "NumGibbsSteps:" ){
        errorLog <<"load(fstream &file) - Failed to read NumGibbsSteps header!" << std::endl;
        return false;
    }
    file >> numGibbsSteps;
    
    //Read the learning rate
    file >> word;
    if( word != "LearningRate:" ){
        errorLog <<"load(fstream &file) - Failed to read LearningRate header!" << std::endl;
        return false;
    }
    file >> learningRate;
    
    //Read the learning rate update
    file >> word;
    if( word != "LearningRateUpdate:" ){
        errorLog <<"load(fstream &file) - Failed to read LearningRateUpdate header!" << std::endl;
        return false;
    }
    file >> learningRateUpdate;
    
    //Read the momentum
    file >> word;
    if( word != "Momentum:" ){
        errorLog <<"load(fstream &file) - Failed to read Momentum header!" << std::endl;
        return false;
    }
    file >> momentum;
    
    //Read the randomizeWeightsForTraining
    file >> word;
    if( word != "RandomizeWeightsForTraining:" ){
        errorLog <<"load(fstream &file) - Failed to read RandomizeWeightsForTraining header!" << std::endl;
        return false;
    }
    file >> randomizeWeightsForTraining;
    
    //Read the ranges
    file >> word;
    if( word != "Ranges:" ){
        errorLog <<"load(fstream &file) - Failed to read Ranges header!" << std::endl;
        return false;
    }
    ranges.resize(numInputDimensions);
    for(UINT n=0; n<ranges.size(); n++){
        file >> ranges[n].minValue;
        file >> ranges[n].maxValue;
    }
    
    //If the model has been trained then load the model
    if( trained ){
        
        //Load the weights matrix
        file >> word;
        if( word != "WeightsMatrix:" ){
            errorLog <<"load(fstream &file) - Failed to read WeightsMatrix header!" << std::endl;
            return false;
        }
        weightsMatrix.resize(numHiddenUnits, numVisibleUnits);
        
        for(UINT i=0; i<weightsMatrix.getNumRows(); i++){
            for(UINT j=0; j<weightsMatrix.getNumCols(); j++){
                file >> weightsMatrix[i][j];
            }
        }
        
        //Load the VisibleLayerBias
        file >> word;
        if( word != "VisibleLayerBias:" ){
            errorLog <<"load(fstream &file) - Failed to read VisibleLayerBias header!" << std::endl;
            return false;
        }
        visibleLayerBias.resize(numVisibleUnits);
        
        for(unsigned int i=0; i<visibleLayerBias.getSize(); i++){
            file >> visibleLayerBias[i];
        }
        
        //Load the HiddenLayerBias
        file >> word;
        if( word != "HiddenLayerBias:" ){
            errorLog <<"load(fstream &file) - Failed to read HiddenLayerBias header!" << std::endl;
            return false;
        }
        hiddenLayerBias.resize(numHiddenUnits);
        
        for(unsigned int i=0; i<hiddenLayerBias.getSize(); i++){
            file >> hiddenLayerBias[i];
        }
    }
    
    return true;
}

GRT_END_NAMESPACE
