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

#include "SVM.h"

namespace GRT{
    
//Register the SVM module with the Classifier base class
RegisterClassifierModule< SVM > SVM::registerModule("SVM");

SVM::SVM(UINT kernelType,UINT svmType,bool useScaling,bool useNullRejection,bool useAutoGamma,double gamma,UINT degree,double coef0,double nu,double C,bool useCrossValidation,UINT kFoldValue){
    
    //Setup the default SVM parameters
    model = NULL;
	param.weight_label = NULL;
	param.weight = NULL;
	trained = false;
	problemSet = false;
	param.svm_type = C_SVC;
	param.kernel_type = LINEAR_KERNEL;
	param.degree = 3;
	param.gamma = 0;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	this->useScaling = false;
	this->useCrossValidation = false;
	this->useNullRejection = false;
	this->useAutoGamma = true;
    classificationThreshold = 0.5;
	crossValidationResult = 0;
    
    classType = "SVM";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG SVM]");
    errorLog.setProceedingText("[ERROR SVM]");
    trainingLog.setProceedingText("[TRAINING SVM]");
    warningLog.setProceedingText("[WARNING SVM]");
    
    init(kernelType,svmType,useScaling,useNullRejection,useAutoGamma,gamma,degree,coef0,nu,C,useCrossValidation,kFoldValue);
}
    
SVM::SVM(const SVM &rhs){
    model = NULL;
	param.weight_label = NULL;
	param.weight = NULL;
    classType = "SVM";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG SVM]");
    errorLog.setProceedingText("[ERROR SVM]");
    trainingLog.setProceedingText("[TRAINING SVM]");
    warningLog.setProceedingText("[WARNING SVM]");
    *this = rhs;
}


SVM::~SVM(){
	clear();
}
    
SVM& SVM::operator=(const SVM &rhs){
    if( this != &rhs ){
        
        this->clear();
        
        //SVM variables
        this->problemSet = rhs.problemSet;
        this->model = rhs.deepCopyModel();
        this->param = rhs.param;
        this->prob = rhs.prob;
        this->numInputDimensions = rhs.numInputDimensions;
        this->kFoldValue = rhs.kFoldValue;
        this->classificationThreshold = rhs.classificationThreshold;
        this->crossValidationResult = rhs.crossValidationResult;
        this->useAutoGamma = rhs.useAutoGamma;
        this->useCrossValidation = rhs.useCrossValidation;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}
    
bool SVM::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        SVM *ptr = (SVM*)classifier;
        
        this->clear();
        
        //SVM variables
        this->problemSet = ptr->problemSet;
        this->model = ptr->deepCopyModel();
        this->param = ptr->param;
        this->prob = ptr->prob;
        this->numInputDimensions = ptr->numInputDimensions;
        this->kFoldValue = ptr->kFoldValue;
        this->classificationThreshold = ptr->classificationThreshold;
        this->crossValidationResult = ptr->crossValidationResult;
        this->useAutoGamma = ptr->useAutoGamma;
        this->useCrossValidation = ptr->useCrossValidation;
        
        //Classifier variables
        return copyBaseVariables( classifier );
    }
    
    return false;
}
    
bool SVM::train_(ClassificationData &trainingData){
    
    //Clear any previous model
    clear();
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    //Convert the labelled classification data into the LIBSVM data format
    if( !convertClassificationDataToLIBSVMFormat(trainingData) ){
        errorLog << "train_(ClassificationData &trainingData) - Failed To Convert Labelled Classification Data To LIBSVM Format!" << endl;
        return false;
    }
    
    if( useAutoGamma ) param.gamma = 1.0/numInputDimensions;
    
	//Train the model
	bool trainingResult = trainSVM();
    
	if(! trainingResult ){
        errorLog << "train_(ClassificationData &trainingData) - Failed To Train SVM Model!" << endl;
		return false;
	}
    
    return true;
}

bool SVM::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - The SVM model has not been trained!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the number of features of the model (" << numInputDimensions << ")" << endl;
        return false;
    }
    
    if( param.probability == 1 ){
        if( !predictSVM( inputVector, maxLikelihood, classLikelihoods ) ){
            errorLog << "predict(VectorDouble inputVector) - Prediction Failed!" << endl;
            return false;
        }
    }else{
        if( !predictSVM( inputVector ) ){
            errorLog << "predict(VectorDouble inputVector) - Prediction Failed!" << endl;
            return false;
        }
    }
    
    return true;
}
    
bool SVM::init(UINT kernelType,UINT svmType,bool useScaling,bool useNullRejection,bool useAutoGamma,double gamma,UINT degree,double coef0,double nu,double C,bool useCrossValidation,UINT kFoldValue){
    
    //Clear any previous models or problems
    clear();
    
    //Validate the kernerlType
    if( !validateKernelType(kernelType) ){
        errorLog << "init(...) - Unknown kernelType!\n";
        return false;
    }
    
    if( !validateSVMType(svmType) ){
        errorLog << "init(...) - Unknown kernelType!\n";
        return false;
    }
    
    param.svm_type = (int)svmType;
	param.kernel_type = (int)kernelType;
	param.degree = (int)degree;
	param.gamma = gamma;
	param.coef0 = coef0;
	param.nu = nu;
	param.cache_size = 100;
	param.C = C;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	this->useScaling = useScaling;
	this->useCrossValidation = useCrossValidation;
	this->useNullRejection = useNullRejection;
	this->useAutoGamma = useAutoGamma;
    classificationThreshold = 0.5;
	crossValidationResult = 0;

    return true;
}
    
void SVM::deleteProblemSet(){
    if( problemSet ){
        for(int i=0; i<prob.l; i++){
            delete[] prob.x[i];
            prob.x[i] = NULL;
        }
        delete[] prob.x;
        delete[] prob.y;
        prob.l = 0;
        prob.x = NULL;
        prob.y = NULL;
        problemSet = false;
    }
}

void SVM::initDefaultSVMSettings(){

	//Clear any previous models, parameters or probelms
	clear();

	//Setup the SVM parameters
	param.svm_type = C_SVC;
	param.kernel_type = LINEAR_KERNEL;
	param.degree = 3;
	param.gamma = 0;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	useCrossValidation = false;
	kFoldValue = 10;
	useAutoGamma = true;
}

bool SVM::validateProblemAndParameters(){
	//Check the parameters match the problem
	const char *errorMsg = svm_check_parameter(&prob,&param);

	if( errorMsg ){
        errorLog << "validateProblemAndParameters() - Parameters do not match problem!" << endl;
		return false;
	}

	return true;
}

bool SVM::trainSVM(){

    crossValidationResult = 0;

    //Erase any previous models
    if( trained ){
        svm_free_and_destroy_model(&model);
        trained = false;
    }

    //Check to make sure the problem has been set
    if( !problemSet ){
        errorLog << "trainSVM() - Problem not set!" << endl;
        return false;
    }
    
    //Verify the problem and the parameters
    if( !validateProblemAndParameters() ) return false;

    //Scale the training data if needed
    if( useScaling ){
        for(int i=0; i<prob.l; i++)
            for(UINT j=0; j<numInputDimensions; j++)
                prob.x[i][j].value = scale(prob.x[i][j].value,ranges[j].minValue,ranges[j].maxValue,SVM_MIN_SCALE_RANGE,SVM_MAX_SCALE_RANGE);
    }

    if( useCrossValidation ){
        int i;
        double total_correct = 0;
        double total_error = 0;
        double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
        double *target = new double[prob.l];

        svm_cross_validation(&prob,&param,kFoldValue,target);
        if( param.svm_type == EPSILON_SVR || param.svm_type == NU_SVR )
        {
            for(i=0;i<prob.l;i++)
            {
                double y = prob.y[i];
                double v = target[i];
                total_error += (v-y)*(v-y);
                sumv += v;
                sumy += y;
                sumvv += v*v;
                sumyy += y*y;
                sumvy += v*y;
            }
            crossValidationResult = total_error/prob.l;
        }
        else
        {
            for(i=0;i<prob.l;i++){
                if(target[i] == prob.y[i]){
                    ++total_correct;
                }
            }
            crossValidationResult = total_correct/prob.l*100.0;
        }
        delete[] target;
    }
        
    //Train the SVM - if we are running cross validation then the CV will be run first followed by a full train
    model = svm_train(&prob,&param);

    if( model == NULL ){
        errorLog << "trainSVM() - Failed to train SVM Model!" << endl;
        return false;
    }

    if( model != NULL ){
        trained = true;
        numClasses = getNumClasses();
        classLabels.resize( getNumClasses() );
        for(UINT k=0; k<getNumClasses(); k++){
            classLabels[k] = model->label[k];
        }
    }

    return trained;
}
    
bool SVM::predictSVM(VectorDouble &inputVector){

		if( !trained || inputVector.size() != numInputDimensions ) return false;

		svm_node *x = NULL;

		//Copy the input data into the SVM format
		x = new svm_node[numInputDimensions+1];
		for(UINT j=0; j<numInputDimensions; j++){
			x[j].index = (int)j+1;
			x[j].value = inputVector[j];
		}
		//The last value in the input vector must be set to -1
		x[numInputDimensions].index = -1;
		x[numInputDimensions].value = 0;

		//Scale the input data if required
		if( useScaling ){
			for(UINT i=0; i<numInputDimensions; i++)
				x[i].value = scale(x[i].value,ranges[i].minValue,ranges[i].maxValue,SVM_MIN_SCALE_RANGE,SVM_MAX_SCALE_RANGE);
		}

		//Perform the SVM prediction
		double predict_label = svm_predict(model,x);

        //We can't do null rejection without the probabilities, so just set the predicted class
        predictedClassLabel = (UINT)predict_label;

		//Clean up the memory
		delete[] x;

		return true;
}

bool SVM::predictSVM(VectorDouble &inputVector,double &maxProbability, VectorDouble &probabilites){

		if( !trained || param.probability == 0 || inputVector.size() != numInputDimensions ) return false;

		double *prob_estimates = NULL;
		svm_node *x = NULL;

		//Setup the memory for the probability estimates
		prob_estimates = new double[ model->nr_class ];

		//Copy the input data into the SVM format
		x = new svm_node[numInputDimensions+1];
		for(UINT j=0; j<numInputDimensions; j++){
			x[j].index = (int)j+1;
			x[j].value = inputVector[j];
		}
		//The last value in the input vector must be set to -1
		x[numInputDimensions].index = -1;
		x[numInputDimensions].value = 0;

		//Scale the input data if required
		if( useScaling ){
			for(UINT j=0; j<numInputDimensions; j++)
				x[j].value = scale(x[j].value,ranges[j].minValue,ranges[j].maxValue,SVM_MIN_SCALE_RANGE,SVM_MAX_SCALE_RANGE);
		}

		//Perform the SVM prediction
		double predict_label = svm_predict_probability(model,x,prob_estimates);

		predictedClassLabel = 0;
		maxProbability = 0;
		probabilites.resize(model->nr_class);
		for(int k=0; k<model->nr_class; k++){
			if( maxProbability < prob_estimates[k] ){
				maxProbability = prob_estimates[k];
                predictedClassLabel = k+1;
                maxLikelihood = maxProbability;
            }
			probabilites[k] = prob_estimates[k];
		}

        if( !useNullRejection ) predictedClassLabel = (UINT)predict_label;
        else{
            if( maxProbability >= classificationThreshold ){
                predictedClassLabel = (UINT)predict_label;
            }else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
        }

		//Clean up the memory
		delete[] prob_estimates;
		delete[] x;

		return true;
}
    
bool SVM::convertClassificationDataToLIBSVMFormat(ClassificationData &trainingData){
    
    //clear any previous problems
    deleteProblemSet();
    
    const UINT numTrainingExamples = trainingData.getNumSamples();
    numInputDimensions = trainingData.getNumDimensions();
    
    //Compute the ranges encase the data should be scaled
    ranges = trainingData.getRanges();
    
    //Init the memory
    prob.l = numTrainingExamples;
    prob.x = new svm_node*[numTrainingExamples];
    prob.y = new double[numTrainingExamples];
    problemSet = true;
    
    for(UINT i=0; i<numTrainingExamples; i++){
        //Set the class ID
        prob.y[i] = trainingData[i].getClassLabel();
        
        //Assign the memory for this training example, note that a dummy node is needed at the end of the vector
        prob.x[i] = new svm_node[numInputDimensions+1];
        for(UINT j=0; j<numInputDimensions; j++){
            prob.x[i][j].index = j+1;
            prob.x[i][j].value = trainingData[i].getSample()[j];
        }
        prob.x[i][numInputDimensions].index = -1; //Assign the final node value
        prob.x[i][numInputDimensions].value = 0;
    }
    
    return true;
}

bool SVM::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        return false;
    }
    
    file << "SVM_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    const svm_parameter& param = trained ? model->param : this->param;
    
    file << "ModelType: ";
    switch( param.svm_type ){
        case(C_SVC):
            file << "C_SVC";
            break;
        case(NU_SVC):
            file << "NU_SVC";
            break;
        case(ONE_CLASS):
            file << "ONE_CLASS";
            break;
        case(EPSILON_SVR):
            file << "EPSILON_SVR";
            break;
        case(NU_SVR):
            file << "NU_SVR";
            break;
        default:
            errorLog << "saveModelToFile(fstream &file) - Invalid model type: " << param.svm_type << endl;
            return false;
            break;
    }
    file << endl;
    
    file << "KernelType: ";
    switch(param.kernel_type){
        case(LINEAR):
            file << "LINEAR";
            break;
        case(POLY):
            file << "POLYNOMIAL";
            break;
        case(RBF):
            file << "RBF";
            break;
        case(SIGMOID):
            file << "SIGMOID";
            break;
        case(PRECOMPUTED):
            file << "PRECOMPUTED";
            break;
        default:
            errorLog << "saveModelToFile(fstream &file) - Invalid kernel type: " << param.kernel_type << endl;
            return false;
            break;
    }
    file << endl;
    file << "Degree: " << param.degree << endl;
    file << "Gamma: " << param.gamma << endl;
    file << "Coef0: " << param.coef0 << endl;
    file << "NumberOfFeatures: " << numInputDimensions << endl;
    file << "UseShrinking: " << param.shrinking << endl;
    file << "UseProbability: " << param.probability << endl;
    
    if( trained ){
        UINT numClasses = (UINT)model->nr_class;
        UINT numSV = (UINT)model->l;
        file << "NumberOfSupportVectors: " << numSV << endl;
    
        file << "RHO: \n";
        for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file << model->rho[i] << "\t";
        file << "\n";
        
        if(model->label){
            file << "Label: \n";
            for(UINT i=0;i<numClasses;i++) file << model->label[i] << "\t";
            file << "\n";
        }
        
        if(model->probA){ // regression has probA only
            file << "ProbA: \n";
            for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file << model->probA[i] << "\t";
            file << "\n";
        }
        
        if(model->probB){
            file << "ProbB: \n";
            for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file << model->probB[i] << "\t";
            file << "\n";
        }
        
        if(model->nSV){
            file << "NumSupportVectorsPerClass: \n";
            for(UINT i=0;i<numClasses;i++) file << model->nSV[i] << "\t";
            file << "\n";
        }
        
        file << "SupportVectors: \n";
        
        const double * const *sv_coef = model->sv_coef;
        const svm_node * const *SV = model->SV;
        
        for(UINT i=0;i<numSV;i++){
            for(UINT j=0;j<numClasses-1;j++)
                file << sv_coef[j][i] << "\t";
            
            const svm_node *p = SV[i];
            
            if(param.kernel_type == PRECOMPUTED) file << (int) p->value << "\t";
            else{
                while(p->index != -1){
                    file << p->index << "\t" << p->value << "\t";
                    p++;
                }
                file << "\n";
            }
        }
    }
    
    return true;
}
    
bool SVM::loadModelFromFile(fstream &file){
    
    string word;
    UINT numSV = 0;
    UINT halfNumClasses = 0;
    
    //Clear any previous models, parameters or problems
    clear();
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Read the file header
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "SVM_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Check to make sure this is a file with the correct File Format
    if( word != "SVM_MODEL_FILE_V2.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        clear();
        return false;
    }

    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    //Init the memory for the model
    model = new svm_model;
    model->nr_class = 0;
    model->l = 0;
    model->SV = NULL;
    model->sv_coef = NULL;
    model->rho = NULL;
    model->probA = NULL;
    model->probB = NULL;
    model->label = NULL;
    model->nSV = NULL;
    model->label = NULL;
    model->nSV = NULL;
    model->free_sv = 0; //This will be set to 1 if everything is loaded OK
    
    //Init the memory for the parameters
    model->param.svm_type = 0;
    model->param.kernel_type = 0;
    model->param.degree = 0;
    model->param.gamma = 0;
    model->param.coef0 = 0;
    model->param.cache_size = 0;
    model->param.eps = 0;
    model->param.C = 0;
    model->param.nr_weight = 0;
    model->param.weight_label = NULL;
    model->param.weight = NULL;
    model->param.nu = 0;
    model->param.p = 0;
    model->param.shrinking = 0;
    model->param.probability = 1;

    //Load the model type
    file >> word;
    if(word != "ModelType:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find ModelType header!" << endl;
        clear();
        return false;
    }
    file >> word;
    if( word == "C_SVC" ){
        model->param.svm_type = C_SVC;
    }else{
        if( word == "NU_SVC" ){
            model->param.svm_type = NU_SVC;
        }else{
            if( word == "ONE_CLASS" ){
                model->param.svm_type = ONE_CLASS; 
            }else{
                if( word == "EPSILON_SVR" ){
                    model->param.svm_type = EPSILON_SVR;
                }else{
                    if( word == "NU_SVR" ){
                        model->param.svm_type = NU_SVR;
                    }else{
                        errorLog << "loadModelFromFile(fstream &file) - Failed to find SVM type!" << endl;
                        clear();
                        return false;
                    }
                }
            }
        }
    }
    
    //Load the model type
    file >> word;
    if(word != "KernelType:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find kernel type!" << endl;
        clear();
        return false;
    }
    file >> word;
    if( word == "LINEAR" ){
        model->param.kernel_type = LINEAR;
    }else{
        if( word == "POLYNOMIAL" ){
            model->param.kernel_type = POLY;
        }else{
            if( word == "RBF" ){
                model->param.kernel_type = RBF; 
            }else{
                if( word == "SIGMOID" ){
                    model->param.kernel_type = SIGMOID;
                }else{
                    if( word == "PRECOMPUTED" ){
                        model->param.kernel_type = PRECOMPUTED;
                    }else{
                        errorLog << "loadModelFromFile(fstream &file) - Failed to find kernel type!" << endl;
                        clear();
                        return false;
                    }
                }
            }
        }
    }
    
    //Load the degree
    file >> word;
    if(word != "Degree:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find Degree header!" << endl;
        clear();
        return false;
    }
    file >> model->param.degree;
    
    //Load the gamma
    file >> word;
    if(word != "Gamma:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find Gamma header!" << endl;
        clear();
        return false;
    }
    file >> model->param.gamma;
    
    //Load the Coef0
    file >> word;
    if(word != "Coef0:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find Coef0 header!" << endl;
        clear();
        return false;
    }
    file >> model->param.coef0;
    
    //Load the NumberOfFeatures
    file >> word;
    if(word != "NumberOfFeatures:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find NumberOfFeatures header!" << endl;
        clear();
        return false;
    }
    file >> numInputDimensions;
    
    //Load the UseShrinking
    file >> word;
    if(word != "UseShrinking:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find UseShrinking header!" << endl;
        clear();
        return false;
    }
    file >> model->param.shrinking;
    
    //Load the UseProbability
    file >> word;
    if(word != "UseProbability:"){
        errorLog << "loadModelFromFile(fstream &file) - Failed to find UseProbability header!" << endl;
        clear();
        return false;
    }
    file >> model->param.probability;
    
    if( trained ){
        //Load the NumberOfSupportVectors
        file >> word;
        if(word != "NumberOfSupportVectors:"){
            errorLog << "loadModelFromFile(fstream &file) - Failed to find NumberOfSupportVectors header!" << endl;
            clear();
            return false;
        }
        file >> numSV;
        
        //Setup the values
        halfNumClasses = numClasses*(numClasses-1)/2;
        model->nr_class = numClasses;
        model->l = numSV;
        
        //Load the RHO
        file >> word;
        if(word != "RHO:"){
            errorLog << "loadModelFromFile(fstream &file) - Failed to find RHO header!" << endl;
            clear();
            return false;
        }
        model->rho = new double[ halfNumClasses ];
        for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file >> model->rho[i];
        
        //See if we can load the Labels
        file >> word;
        if(word != "Label:"){
            model->label = NULL;
        }else{
            model->label = new int[ numClasses ];
            for(UINT i=0;i<numClasses;i++) file >> model->label[i];
            //We only need to read a new line if we found the label!
            file >> word;
        }
        
        //See if we can load the ProbA
        //We don't need to read another line here
        if(word != "ProbA:"){
            model->probA = NULL;
        }else{
            model->probA = new double[ halfNumClasses ];
            for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file >> model->probA[i];
            //We only need to read a new line if we found the label!
            file >> word;
        }
        
        //See if we can load the ProbB
        //We don't need to read another line here
        if(word != "ProbB:"){
            model->probB = NULL;
        }else{
            model->probB = new double[ halfNumClasses ];
            for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file >> model->probB[i];
            //We only need to read a new line if we found the label!
            file >> word;
        }
        
        //See if we can load the NumSupportVectorsPerClass
        //We don't need to read another line here
        if( word == "NumSupportVectorsPerClass:" ){
            model->nSV = new int[ numClasses ];
            for(UINT i=0; i<numClasses; i++) file >> model->nSV[i];
            //We only need to read a new line if we found the label!
            file >> word;
        }else{
            model->nSV = NULL;
        }
        
        //Load the SupportVectors
        //We don't need to read another line here
        if(word != "SupportVectors:"){
            errorLog << "loadModelFromFile(fstream &file) - Failed to find SupportVectors header!" << endl;
            clear();
            return false;
        }
        
        //Setup the memory
        model->sv_coef = new double*[numClasses-1];
        for(UINT j=0;j<numClasses-1;j++) model->sv_coef[j] = new double[numSV];
        model->SV = new svm_node*[numSV];
        
        for(UINT i=0; i<numSV; i++){
            for(UINT j=0; j<numClasses-1; j++){
                file >> model->sv_coef[j][i];
            }
            
            model->SV[i] = new svm_node[numInputDimensions+1];
            
            if(model->param.kernel_type == PRECOMPUTED) file >> model->SV[i][0].value;
            else{
                for(UINT j=0; j<numInputDimensions; j++){
                    file >> model->SV[i][j].index;
                    file >> model->SV[i][j].value;
                }
                model->SV[i][numInputDimensions].index = -1; //Assign the final node value
                model->SV[i][numInputDimensions].value = 0;
            }
        }
        
        //Set the class labels
        this->numClasses = getNumClasses();
        classLabels.resize(getNumClasses());
        for(UINT k=0; k<getNumClasses(); k++){
            classLabels[k] = model->label[k];
        }
        
        //The SV have now been loaded so flag that they should be deleted
        model->free_sv = 1;
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}
    
bool SVM::clear(){
    
    //Clear the base class
    Classifier::clear();
    
    crossValidationResult = 0;
    trained = false;
    svm_free_and_destroy_model(&model);
    svm_destroy_param(&param);
    deleteProblemSet();
    
    return true;
}
    
bool SVM::getIsCrossValidationTrainingEnabled() const{
    return useCrossValidation;
}
    
bool SVM::getIsAutoGammaEnabled() const{
    return useAutoGamma;
}
    
string SVM::getSVMType() const{

	const struct svm_parameter *paramPtr = NULL;
	string modelName = "UNKNOWN";
	if( trained ){
		paramPtr = &model->param;
	}else paramPtr = &param;

	switch(paramPtr->svm_type){
			case(C_SVC):
				modelName =  "C_SVC";
				break;
			case(NU_SVC):
				modelName = "NU_SVC";
				break;
			case(ONE_CLASS):
				modelName = "ONE_CLASS";
				break;
			case(EPSILON_SVR):
				modelName = "EPSILON_SVR";
				break;
			case(NU_SVR):
				modelName = "NU_SVR";
				break;
			default:
				break;
	}

	return modelName;
}
    
string SVM::getKernelType() const{
    const struct svm_parameter *paramPtr = NULL;
    string modelName = "UNKNOWN";
    if( trained ){
        paramPtr = &model->param;
    }else paramPtr = &param;
    
    switch(paramPtr->kernel_type){
        case(LINEAR_KERNEL):
            modelName =  "LINEAR_KERNEL";
            break;
        case(POLY_KERNEL):
            modelName = "POLY_KERNEL";
            break;
        case(RBF_KERNEL):
            modelName = "RBF_KERNEL";
            break;
        case(SIGMOID_KERNEL):
            modelName = "SIGMOID_KERNEL";
            break;
        case(PRECOMPUTED_KERNEL):
            modelName = "PRECOMPUTED_KERNEL";
            break;
        default:
            break;
    }
    return modelName;
}
    
UINT SVM::getNumClasses() const{
    if( !trained ) return 0;
    return (UINT) model->nr_class;
}
    
UINT SVM::getDegree() const{
    if( trained ){
        return (UINT)model->param.degree;
    }
    return (UINT)param.gamma;
}
    
double SVM::getGamma() const{
    if( trained ){
        return model->param.gamma;
    }
    return param.gamma;
}
    
double SVM::getNu() const{
    if( trained ){
        return model->param.nu;
    }
    return param.gamma;
}

double SVM::getCoef0() const{
    if( trained ){
        return model->param.coef0;
    }
    return param.gamma;
}

double SVM::getC() const{
    if( trained ){
        return model->param.C;
    }
    return param.gamma;
}
    
double SVM::getCrossValidationResult() const{ return crossValidationResult; }

bool SVM::setSVMType(const UINT svmType){
    if( validateSVMType(svmType) ){
        param.svm_type = (int)svmType;
        return true;
    }
    return false;
}
    
bool SVM::setKernelType(const UINT kernelType){
    if( validateKernelType(kernelType) ){
        param.kernel_type = (int)kernelType;
        return true;
    }
    warningLog << "setKernelType(UINT kernelType) - Failed to set kernel type, unknown kernelType!" << endl;
    return false;
}
    
bool SVM::setGamma(const double gamma){
    if( !useAutoGamma ){
        this->param.gamma = gamma;
        return true;
    }
    warningLog << "setGamma(double gamma) - Failed to set gamma, useAutoGamma is enabled, setUseAutoGamma to false first!" << endl;
    return false;
}
    
bool SVM::setDegree(const UINT degree){
    this->param.degree = (int)degree;
    return true;
}
    
bool SVM::setNu(const double nu){
    this->param.nu = nu;
    return true;
}

bool SVM::setCoef0(const double coef0){
    this->param.coef0 = coef0;
    return true;
}

bool SVM::setC(const double C){
    this->param.C = C;
    return true;
}
    
bool SVM::setKFoldCrossValidationValue(const UINT kFoldValue){
    if( kFoldValue > 0 ){
        this->kFoldValue = kFoldValue;
        return true;
    }
    warningLog << "setKFoldCrossValidationValue(const UINT kFoldValue) - Failed to set kFoldValue, the kFoldValue must be greater than 0!" << endl;
    return false;
}
    
bool SVM::enableAutoGamma(const bool useAutoGamma){
    this->useAutoGamma = useAutoGamma;
    return true;
}

bool SVM::enableCrossValidationTraining(const bool useCrossValidation){
    this->useCrossValidation = useCrossValidation;
    return true;
}
    
bool SVM::validateSVMType(const UINT svmType){
    if( svmType == C_SVC ){
        return true;
    }
    if( svmType == NU_SVC ){
        return true;
    }
    if( svmType == ONE_CLASS ){
        return true;
    }
    if( svmType == EPSILON_SVR ){
        return true;
    }
    if( svmType == NU_SVR ){
        return true;
    }
    return false;
}
    
bool SVM::validateKernelType(const UINT kernelType){
    if( kernelType == LINEAR_KERNEL ){
        return true;
    }
    if( kernelType == POLY_KERNEL ){
        return true;
    }
    if( kernelType == RBF_KERNEL ){
        return true;
    }
    if( kernelType == SIGMOID_KERNEL ){
        return true;
    }
    if( kernelType == PRECOMPUTED_KERNEL ){
        return true;
    }
    return false;
}
    
struct svm_model* SVM::deepCopyModel() const{
    
    if( model == NULL ) return NULL;
    
    UINT halfNumClasses = 0;
    
    //Init the memory for the model
    struct svm_model *m;
    m = new svm_model;
    m->nr_class = 0;
    m->l = 0;
    m->SV = NULL;
    m->sv_coef = NULL;
    m->rho = NULL;
    m->probA = NULL;
    m->probB = NULL;
    m->label = NULL;
    m->nSV = NULL;
    m->label = NULL;
    m->nSV = NULL;
    m->free_sv = 0; //This will be set to 1 if everything is loaded OK
    
    //Init the memory for the parameters
    m->param.svm_type = 0;
    m->param.kernel_type = 0;
    m->param.degree = 0;
    m->param.gamma = 0;
    m->param.coef0 = 0;
    m->param.cache_size = 0;
    m->param.eps = 0;
    m->param.C = 0;
    m->param.nr_weight = 0;
    m->param.weight_label = NULL;
    m->param.weight = NULL;
    m->param.nu = 0;
    m->param.p = 0;
    m->param.shrinking = 0;
    m->param.probability = 1;
    
    //Copy the parameters
    m->param.svm_type = model->param.svm_type;
    m->param.kernel_type = model->param.kernel_type ;
    m->param.degree = model->param.degree;
    m->param.gamma = model->param.gamma;
    m->param.coef0 = model->param.coef0;
    m->nr_class = model->nr_class;
    m->l = model->l;
    m->param.shrinking = model->param.shrinking;
    m->param.probability = model->param.probability;
    
    //Setup the values
    halfNumClasses = model->nr_class*(model->nr_class-1)/2;

    m->rho = new double[ halfNumClasses ];
    for(int i=0;i <model->nr_class*(model->nr_class-1)/2; i++) m->rho[i] = model->rho[i];
    
    if( model->label != NULL ){
        m->label = new int[ model->nr_class ];
        for(int i=0;i<model->nr_class;i++) m->label[i] = model->label[i];
    }
    
    if( model->probA != NULL ){
        m->probA = new double[ halfNumClasses ];
        for(UINT i=0;i<halfNumClasses; i++) m->probA[i] = model->probA[i];
    }
    
    if( model->probB != NULL ){
        m->probB = new double[ halfNumClasses ];
        for(UINT i=0; i<halfNumClasses; i++) m->probB[i] = model->probB[i];
    }
    
    if( model->nSV != NULL ){
        m->nSV = new int[ model->nr_class ];
        for(int i=0; i<model->nr_class; i++) m->nSV[i] = model->nSV[i];
    }
    
    //Setup the memory
    m->sv_coef = new double*[numClasses-1];
    for(UINT j=0;j<numClasses-1;j++) m->sv_coef[j] = new double[model->l];
    m->SV = new svm_node*[model->l];
    
    for(int i=0; i<model->l; i++){
        for(int j=0; j<model->nr_class-1; j++){
            m->sv_coef[j][i] = model->sv_coef[j][i];
        }
        
        m->SV[i] = new svm_node[numInputDimensions+1];
        
        if(model->param.kernel_type == PRECOMPUTED) m->SV[i][0].value = model->SV[i][0].value;
        else{
            for(UINT j=0; j<numInputDimensions; j++){
                m->SV[i][j].index =  model->SV[i][j].index;
                m->SV[i][j].value = model->SV[i][j].value;
            }
            m->SV[i][numInputDimensions].index = -1; //Assign the final node value
            m->SV[i][numInputDimensions].value = 0;
        }
    }
    
    //The SV have now been loaded so flag that they should be deleted
    m->free_sv = 1;
    
    return m;
    
}
    
bool SVM::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    UINT numSV = 0;
    UINT halfNumClasses = 0;
    numInputDimensions = 0;
    
    //Init the memory for the model
    model = new svm_model;
    model->nr_class = 0;
    model->l = 0;
    model->SV = NULL;
    model->sv_coef = NULL;
    model->rho = NULL;
    model->probA = NULL;
    model->probB = NULL;
    model->label = NULL;
    model->nSV = NULL;
    model->label = NULL;
    model->nSV = NULL;
    model->free_sv = 0; //This will be set to 1 if everything is loaded OK
    
    //Init the memory for the parameters
    model->param.svm_type = 0;
    model->param.kernel_type = 0;
    model->param.degree = 0;
    model->param.gamma = 0;
    model->param.coef0 = 0;
    model->param.cache_size = 0;
    model->param.eps = 0;
    model->param.C = 0;
    model->param.nr_weight = 0;
    model->param.weight_label = NULL;
    model->param.weight = NULL;
    model->param.nu = 0;
    model->param.p = 0;
    model->param.shrinking = 0;
    model->param.probability = 1;
    
    //Load the model type
    file >> word;
    if(word != "ModelType:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find ModelType header!" << endl;
        clear();
        return false;
    }
    file >> word;
    if( word == "C_SVC" ){
        model->param.svm_type = C_SVC;
    }else{
        if( word == "NU_SVC" ){
            model->param.svm_type = NU_SVC;
        }else{
            if( word == "ONE_CLASS" ){
                model->param.svm_type = ONE_CLASS;
            }else{
                if( word == "EPSILON_SVR" ){
                    model->param.svm_type = EPSILON_SVR;
                }else{
                    if( word == "NU_SVR" ){
                        model->param.svm_type = NU_SVR;
                    }else{
                        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find SVM type!" << endl;
                        clear();
                        return false;
                    }
                }
            }
        }
    }
    
    //Load the model type
    file >> word;
    if(word != "KernelType:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find kernel type!" << endl;
        clear();
        return false;
    }
    file >> word;
    if( word == "LINEAR" ){
        model->param.kernel_type = LINEAR;
    }else{
        if( word == "POLYNOMIAL" ){
            model->param.kernel_type = POLY;
        }else{
            if( word == "RBF" ){
                model->param.kernel_type = RBF;
            }else{
                if( word == "SIGMOID" ){
                    model->param.kernel_type = SIGMOID;
                }else{
                    if( word == "PRECOMPUTED" ){
                        model->param.kernel_type = PRECOMPUTED;
                    }else{
                        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find kernel type!" << endl;
                        clear();
                        return false;
                    }
                }
            }
        }
    }
    
    //Load the degree
    file >> word;
    if(word != "Degree:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find Degree header!" << endl;
        clear();
        return false;
    }
    file >> model->param.degree;
    
    //Load the gamma
    file >> word;
    if(word != "Gamma:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find Gamma header!" << endl;
        clear();
        return false;
    }
    file >> model->param.gamma;
    
    //Load the Coef0
    file >> word;
    if(word != "Coef0:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find Coef0 header!" << endl;
        clear();
        return false;
    }
    file >> model->param.coef0;
    
    //Load the NumberOfClasses
    file >> word;
    if(word != "NumberOfClasses:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find NumberOfClasses header!" << endl;
        clear();
        return false;
    }
    file >> numClasses;
    
    //Load the NumberOfSupportVectors
    file >> word;
    if(word != "NumberOfSupportVectors:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find NumberOfSupportVectors header!" << endl;
        clear();
        return false;
    }
    file >> numSV;
    
    //Load the NumberOfFeatures
    file >> word;
    if(word != "NumberOfFeatures:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find NumberOfFeatures header!" << endl;
        clear();
        return false;
    }
    file >> numInputDimensions;
    
    //Load the UseShrinking
    file >> word;
    if(word != "UseShrinking:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find UseShrinking header!" << endl;
        clear();
        return false;
    }
    file >> model->param.shrinking;
    
    //Load the UseProbability
    file >> word;
    if(word != "UseProbability:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find UseProbability header!" << endl;
        clear();
        return false;
    }
    file >> model->param.probability;
    
    //Load the UseScaling
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find UseScaling header!" << endl;
        clear();
        return false;
    }
    file >> useScaling;
    
    //Load the Ranges
    file >> word;
    if(word != "Ranges:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find Ranges header!" << endl;
        clear();
        return false;
    }
    //Setup the memory for the ranges
    ranges.clear();
    ranges.resize(numInputDimensions);
    
    ///Load the ranges
    for(UINT i=0; i<ranges.size(); i++){
        file >> ranges[i].minValue;
        file >> ranges[i].maxValue;
    }
    
    //Setup the values
    halfNumClasses = numClasses*(numClasses-1)/2;
    model->nr_class = numClasses;
    model->l = numSV;
    
    //Load the RHO
    file >> word;
    if(word != "RHO:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find RHO header!" << endl;
        clear();
        return false;
    }
    model->rho = new double[ halfNumClasses ];
    for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file >> model->rho[i];
    
    //See if we can load the Labels
    file >> word;
    if(word != "Label:"){
        model->label = NULL;
    }else{
        model->label = new int[ numClasses ];
        for(UINT i=0;i<numClasses;i++) file >> model->label[i];
        //We only need to read a new line if we found the label!
        file >> word;
    }
    
    //See if we can load the ProbA
    //We don't need to read another line here
    if(word != "ProbA:"){
        model->probA = NULL;
    }else{
        model->probA = new double[ halfNumClasses ];
        for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file >> model->probA[i];
        //We only need to read a new line if we found the label!
        file >> word;
    }
    
    //See if we can load the ProbB
    //We don't need to read another line here
    if(word != "ProbB:"){
        model->probB = NULL;
    }else{
        model->probB = new double[ halfNumClasses ];
        for(UINT i=0;i<numClasses*(numClasses-1)/2;i++) file >> model->probB[i];
        //We only need to read a new line if we found the label!
        file >> word;
    }
    
    //See if we can load the NumSupportVectorsPerClass
    //We don't need to read another line here
    if(word != "NumSupportVectorsPerClass:"){
        model->nSV = NULL;
    }else{
        model->nSV = new int[ numClasses ];
        for(UINT i=0;i<numClasses;i++) file >> model->nSV[i];
        //We only need to read a new line if we found the label!
        file >> word;
    }
    
    //Load the SupportVectors
    //We don't need to read another line here
    if(word != "SupportVectors:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Failed to find SupportVectors header!" << endl;
        clear();
        return false;
    }
    
    //Setup the memory
    model->sv_coef = new double*[numClasses-1];
    for(UINT j=0;j<numClasses-1;j++) model->sv_coef[j] = new double[numSV];
    model->SV = new svm_node*[numSV];
    
    for(UINT i=0; i<numSV; i++){
        for(UINT j=0; j<numClasses-1; j++){
            file >> model->sv_coef[j][i];
        }
        
        model->SV[i] = new svm_node[numInputDimensions+1];
        
        if(model->param.kernel_type == PRECOMPUTED) file >> model->SV[i][0].value;
        else{
            for(UINT j=0; j<numInputDimensions; j++){
                file >> model->SV[i][j].index;
                file >> model->SV[i][j].value;
            }
            model->SV[i][numInputDimensions].index = -1; //Assign the final node value
            model->SV[i][numInputDimensions].value = 0;
        }
    }
    
    //Set the class labels
    this->numClasses = getNumClasses();
    classLabels.resize(getNumClasses());
    for(UINT k=0; k<getNumClasses(); k++){
        classLabels[k] = model->label[k];
    }
    
    //The SV have now been loaded so flag that they should be deleted
    model->free_sv = 1;
    
    //Finally, flag that the model has been trained to show it has been loaded and can be used for prediction
    trained = true;
    
    return true;
}
    
} //End of namespace GRT
