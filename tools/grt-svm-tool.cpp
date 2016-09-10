/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for training a Support Vector Machine (SVM) model. The dataset used to train the model can be in two formats, (1) a GRT ClassificationData formatted file or (2)
 a CSV formatted file.  If the data is formatted as a CSV file then it should be formatted as follows:
 - each row should contain a sample [label, data....]
 - the first column should contain the class label (unsigned int)
 - the next N columns should contain the input attributes (a.k.a. features)
 - columns should be seperated by a comma delimiter ','
 - rows should be ended with a new line operator '\n'
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

const char *help = "This tool can be used for training a SVM classification model.\n"
                   "The dataset used to train the model can be in two format, (1) a GRT ClassificationData formatted file or (2) a CSV formatted file.\n"
                   "If the data is formatted as a CSV file, then it should be formatted as follows:\n"
                     "\t- each row should contain a sample\n"
                     "\t- the first column should contain the class label\n"
                     "\t- the remaining N columns should contain the input attributes (a.k.a. features)\n"
                     "\t- columns should be seperated by a comma delimiter \',\'\n"
                     "\t- rows should be terminated by a new line operator \'\\n\'";
                     
InfoLog infoLog("[grt-svm-tool]");
WarningLog warningLog("[WARNING grt-svm-tool]");
ErrorLog errorLog("[ERROR grt-svm-tool]");

bool printHelp(){
  infoLog << "\nusage: grt-svm-tool [options]\n";
  infoLog << "\t--help: prints this help message\n";
  infoLog << "\t-f: sets the filename the training data will be loaded from. The training data can either be a GRT ClassificationData file or a CSV file (see help below)\n";
  infoLog << "\t--model: sets the filename the model will be saved to\n";
  infoLog << "\t--kernel: sets the SVM kernel type, the options are: 'linear', 'poly', 'rbf', 'sigmoid'. Default: 'linear'\n";
  infoLog << "\t--svm-type: sets the SVM type, the options are: 'c_svc', 'nu_svc', 'one_class', 'epsilon_svr', 'nu_svr'. Default: 'c_svc'\n";
  infoLog << "\t--scale-data: set if the training data (and prediction data) should be scaled. Default: true\n";
  infoLog << "\t--gamma: sets the gamma parameter for the SVM learning algorithm. Default: 0.1\n";
  infoLog << "\t--degree: sets the degree parameter for the SVM learning algorithm. Default: 3\n";
  infoLog << "\t--nu: sets the nu parameter for the SVM learning algorithm. Default: 0.5\n";
  infoLog << "\t--C: sets the C parameter for the SVM learning algorithm. Default: 1\n";
  infoLog << "\t--log-file: sets the filename the training log will be saved to, no file will be saved if this option is not used \n";
  infoLog << "\nhelp:\n" << help;
  infoLog << endl;
  return true;
}

bool train( CommandLineParser &parser );

int main(int argc, char * argv[])
{

    if( argc < 2 ){
        errorLog << "Not enough input arguments!" << endl;
        printHelp();
        return EXIT_FAILURE;
    }

    //Create an instance of the parser
    CommandLineParser parser;

    //Disable warning messages
    parser.setWarningLoggingEnabled( false );

    //Add some options and identifiers that can be used to get the results
    parser.addOption( "--help", "help" );
    parser.addOption( "-f", "filename" );
    parser.addOption( "--model", "model-filename", "svm-model.grt" ); //Set the default model name to svm-model.grt
    parser.addOption( "--kernel", "kernel", "linear" ); //Set the default kernel to linear
    parser.addOption( "--svm-type", "svm-type", "c_svc" ); //Set the default svm type to c_svc
    parser.addOption( "--scale-data", "scale-data", true ); //Set the default scale-data option to true
    parser.addOption( "--gamma", "gamma", 0.1 ); //Set the default gamma value to 0.1
    parser.addOption( "--degree", "degree", 3 ); //Set the default degree value to 3
    parser.addOption( "-C", "C", 1 ); //Set the default C value to 1
    parser.addOption( "--nu", "nu", 0.5 ); //Set the default C value to 1
    parser.addOption( "--log", "log-filename" );
  
    //Parse the command line
    parser.parse( argc, argv );

    if( parser.getOptionParsed("help") ){
      printHelp();
      return EXIT_SUCCESS;
    }

    //Train the model model
    if( train( parser ) ){
      infoLog << "Model Trained!" << endl;
      return EXIT_SUCCESS;
    }

    errorLog << "Failed to train model!" << endl;
    printHelp();

    return EXIT_FAILURE;
}

bool train( CommandLineParser &parser ){

    string trainDatasetFilename = "";
    string modelFilename = "";

    //Get the filename
    if( !parser.get("filename",trainDatasetFilename) ){
        errorLog << "Failed to parse filename from command line! You can set the filename using the -f." << endl;
        return false;
    }

    //Get the model name
    if( !parser.get("model-filename",modelFilename) ){
        errorLog << "Failed to parse model filename from command line! You can set the model filename using the --model." << endl;
        return false;
    }

    //Load the training data to train the model
    ClassificationData trainingData;

    infoLog << "- Loading Training Data..." << endl;
    if( !trainingData.load( trainDatasetFilename ) ){
        errorLog << "Failed to load training data!\n";
        return false;
    }

    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    infoLog << "- Num training samples: " << trainingData.getNumSamples() << endl;
    infoLog << "- Num input dimensions: " << N << endl;
    infoLog << "- Num classes: " << K << endl;
   
    string kernelStr = "";
    string svmTypeStr = "";
    SVM::KernelType kernelType = SVM::LINEAR_KERNEL;
    SVM::SVMType svmType = SVM::C_SVC;
    bool scaleData = false;
    Float gamma = 0.0;
    Float coef0 = 0.0;
    Float nu = 0.0;
    UINT degree = 0;
    Float C = 0.0;

    //Get the options from the parser
    parser.get( "kernel", kernelStr );
    parser.get( "svm-type", svmTypeStr );
    parser.get( "scale-data", scaleData );
    parser.get( "gamma", gamma );
    parser.get( "coef0", coef0 );
    parser.get( "nu", nu );
    parser.get( "degree", degree );
    parser.get( "C", C );

    infoLog << "settings: kernel: " << kernelStr << " svm-type: " << svmTypeStr << " scale-data: " << scaleData << " gamma: " << gamma << " degree: " << degree << " nu: " << nu << " C: " << C << endl;


    //Conver the kernel type string to the enum
    if( kernelStr == "" || kernelStr == "linear" ){
        kernelType = SVM::LINEAR_KERNEL;
    }else if( kernelStr == "poly" ){
         kernelType = SVM::POLY_KERNEL;
    }else if( kernelStr == "rbf" ){
         kernelType = SVM::RBF_KERNEL;
    }else if( kernelStr == "sigmoid" ){
         kernelType = SVM::SIGMOID_KERNEL;
    }else{
        errorLog << "Unknown kernel-type: " << kernelStr << endl;
        return false;
    }

    //Create a new SVM instance
    bool enableNullRejection = false;
    bool estimateGamma = false;
    bool useCV = false;
    UINT kFoldValue = 10;
    SVM classifier(kernelType,svmType,scaleData,enableNullRejection,estimateGamma,gamma,degree,coef0,nu,C,useCV,kFoldValue);

    //Create a new pipeline that will hold the classifier
    GestureRecognitionPipeline pipeline;

    //Add the classifier to the pipeline
    pipeline << classifier;

    infoLog << "- Training model...\n";

    //Train the classifier
    if( !pipeline.train( trainingData ) ){
        errorLog << "Failed to train model!" << endl;
        return false;
    }

    infoLog << "- Model trained!" << endl;
    infoLog << "- Training time: " << (pipeline.getTrainingTime() * 0.001) / 60.0 << " (minutes)" << endl;
    infoLog << "- Saving model to: " << modelFilename << endl;

    //Save the pipeline
    if( !pipeline.save( modelFilename ) ){
        warningLog << "Failed to save model to file: " << modelFilename << endl;
        return false;
    } 
    
    string logFilename = "";
    if( parser.get( "log-filename", logFilename ) && logFilename.length() > 0 ){
      infoLog << "Writing training log to: " << logFilename << endl;

      fstream logFile( logFilename.c_str(), fstream::out );

      if( !logFile.is_open() ){
        errorLog << "Failed to open training log file: " << logFilename << endl;
        return false;
      }

      Vector< TrainingResult > trainingResults = pipeline.getTrainingResults();

      for(UINT i=0; i<trainingResults.getSize(); i++){
        logFile << trainingResults[i].getTrainingIteration() << "\t" << trainingResults[i].getAccuracy() << endl;
      }

      logFile.close();
    }

    return true;
}
