/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for training a linear regression model. The dataset used to train the model can be in two formats, (1) a GRT RegressionData formatted file or (2)
 a CSV formatted file.  If the data is formatted as a CSV file then it should be formatted as follows:
 - each row should contain a sample
 - the first N columns should contain the input attributes (a.k.a. features)
 - the last T columns should contain the target attributes
 - columns should be seperated by a comma delimiter ','
 - rows should be ended with a new line operator '\n'
 Note, if the CSV option is used, then the user must also specifiy the number of input dimensions and number of target dimensions via the command line options (-n and -t respectively). These
 additional arguments are not required if the GRT RegressionData file format is used (as this information is contained in the meta data section of the file).
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

const char *help = "This tool implements a basic tool for training a Softmax classification model.\n"
                   "The dataset used to train the model can be in two format, (1) a GRT ClassificationData formatted file or (2) a CSV formatted file.\n"
                   "If the data is formatted as a CSV file, then it should be formatted as follows:\n"
                     "\t- each row should contain a sample\n"
                     "\t- the first column should contain the class label\n"
                     "\t- the remaining N columns should contain the input attributes (a.k.a. features)\n"
                     "\t- columns should be seperated by a comma delimiter \',\'\n"
                     "\t- rows should be terminated by a new line operator \'\\n\'";
                     
InfoLog infoLog("[grt-softmax-tool]");
WarningLog warningLog("[WARNING grt-softmax-tool]");
ErrorLog errorLog("[ERROR grt-softmax-tool]");

bool printHelp(){
  infoLog << "\nusage: grt-softmax-tool [options]\n";
  infoLog << "\t--help: prints this help message\n";
  infoLog << "\t-f: sets the filename the training data will be loaded from. The training data can either be a GRT ClassificationData file or a CSV file (see help below)\n";
  infoLog << "\t--model: sets the filename the Softmax model will be saved to\n";
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
    parser.addOption( "-f", "filename" );
    parser.addOption( "-n", "num-input-dimensions" );
    parser.addOption( "-t", "num-target-dimensions" );
    parser.addOption( "--model", "model-filename" );
    parser.addOption( "--help", "help" );

    //Parse the command line
    parser.parse( argc, argv );

    if( parser.optionParsed("help") ){
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

    infoLog << "Training regression model..." << endl;

    string trainDatasetFilename = "";
    string modelFilename = "";
    string defaultFilename = "linear-regression-model.grt";

    //Get the filename
    if( !parser.get("filename",trainDatasetFilename) ){
        errorLog << "Failed to parse filename from command line! You can set the filename using the -f." << endl;
        printHelp();
        return false;
    }

    //Get the model filename
    parser.get("model-filename",modelFilename,defaultFilename);

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

    //Create a new softmax instance
    bool enableScaling = true;
    float learningRate = 0.01;
    float minChange = 1.0e-5;
    unsigned int maxEpochs = 1000;
    Softmax classifier(enableScaling,learningRate,minChange,maxEpochs);

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

    infoLog << "- Saving model to: " << modelFilename << endl;

    //Save the pipeline
    if( pipeline.save( modelFilename ) ){
        infoLog << "- Model saved." << endl;
    }else warningLog << "Failed to save model to file: " << modelFilename << endl;

    infoLog << "- TrainingTime: " << pipeline.getTrainingTime() << endl;

    return true;
}

