/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for training a logistic regression model. See the help string for more info.
*/

#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

const char *help = "This tool can be used for training a Logistic Regression model. The tool will wrap the Logistic Regression model in the GRT MDRegression algorithm, this enables the number\n"
                   "of target dimensions to be >= 1\n"
                   "The dataset used to train the model can be in three formats, (1) a GRT ClassificationData formatted file, (2) a GRT RegressionData formatted file or (3) a CSV formatted file.\n"
                   "If the data is formatted as a CSV file, then it should be formatted as GRT CSV RegressionData, which is as follows:\n"
                     "\t- each row should contain a sample\n"
                     "\t- the first N columns should contain the input attributes (a.k.a. features)\n"
                     "\t- the last K columns should contain the target attributes (a.k.a. regression targets or classes)\n"
                     "\t- columns should be seperated by a comma delimiter \',\'\n"
                     "\t- rows should be terminated by a new line operator \'\\n\'";
                                          
InfoLog infoLog("[grt-log-reg-tool]");
WarningLog warningLog("[WARNING grt-log-reg-tool]");
ErrorLog errorLog("[ERROR grt-log-reg-tool]");

bool printHelp(){
  infoLog << "\nusage: grt-log-reg-tool [options]\n";
  infoLog << "\t--help: prints this help message\n";
  infoLog << "\t-f: sets the filename the training data will be loaded from. The training data can either be a GRT ClassificationData, a GRT RegressionData file or a CSV file (see help below). \n";
  infoLog << "\t--val-data: sets the filename the validation data will be loaded from, if not set then the validation data will be randomly sampled from the training data.  The validation dataset should be in the same format as the training dataset. \n";
  infoLog << "\t--model: sets the filename the model will be saved to\n";
  infoLog << "\t--batch-size: sets the number of training samples used in each batch update of the learning algorithm (default: 50)\n";
  infoLog << "\t--learning-rate: sets the rate at which the learning algorithm will update the model weights at each batch update (default: 0.01)\n";
  infoLog << "\t--min-change: sets the minimum change needed to signal convergence of the learning algorithm (default: 0.001)\n";
  infoLog << "\t--max-epoch: sets the maximum number of epochs that the learning algorithm can run for, an epoch is one iteration over the entire training dataset (default: 500)\n";
  infoLog << "\nhelp:\n" << help;
  infoLog << endl;
  return true;
}

bool train( CommandLineParser &parser );

int main(int argc, char * argv[])
{

    if( argc < 2 ){
        errorLog << "Not enough input arguments! Run the application with the --help option for information on the expected input arguments." << endl;
        return EXIT_FAILURE;
    }

    //Create an instance of the parser
    CommandLineParser parser;

    //Disable warning messages
    parser.setWarningLoggingEnabled( false );

    //Add some options and identifiers that can be used to get the results
    parser.addOption( "-f", "train-data" );
    parser.addOption( "--val-data", "val-data", "" ); //Set the default validation data filename to be an empty string (which means the validation data will not be loaded and will be randomly sampled from the training data )
    parser.addOption( "--model", "model-filename", "log-regression-model.grt" ); //Set the default filename to log-regression-model.grt
    parser.addOption( "--num-inputs", "num-inputs" ); //The number of features, only required if the input file is a CSV file
    parser.addOption( "--num-targets", "num-targets" ); //The number of targets, only required if the input file is a CSV file
    parser.addOption( "--batch-size", "batch-size", 50 ); //Set the default batch size to 50
    parser.addOption( "--learning-rate", "learning-rate", 0.01 ); //Set the default learning rate to 0.01
    parser.addOption( "--min-change", "min-change", 0.001 ); //Set the default min change to 0.001
    parser.addOption( "--max-epoch", "max-epoch", 500 ); //Set the default max number of epochs to 500
    parser.addOption( "--enable-scaling", "enable-scaling", true ); //Set the default scaling option to true
    parser.addOption( "--log", "log-filename" );

    //Parse the command line
    parser.parse( argc, argv );

    //Train the model model
    if( train( parser ) ){
      return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

bool train( CommandLineParser &parser ){

    infoLog << "training regression model..." << endl;

    string trainDatasetFilename = "";
    string validationDatasetFilename = "";
    string modelFilename = "";
    float learningRate = 0;
    float minChange = 0;
    unsigned int maxEpoch = 0;
    unsigned int batchSize = 0;
    bool enableScaling = true;

    //Get the filename
    if( !parser.get("train-data",trainDatasetFilename) ){
        errorLog << "Failed to parse filename from command line! You can set the filename using the -f." << endl;
        printHelp();
        return false;
    }
    
    //Get the parameters from the parser
    parser.get( "model-filename",modelFilename);
    parser.get( "learning-rate", learningRate );
    parser.get( "min-change", minChange );
    parser.get( "max-epoch", maxEpoch );
    parser.get( "batch-size", batchSize );
    parser.get( "enable-scaling", enableScaling );
    parser.get( "val-data",validationDatasetFilename);

    infoLog << "settings: learning-rate: " << learningRate << " min-change: " << minChange << " max-epoch: " << maxEpoch << " batch-size: " << batchSize << " scale-data: " << enableScaling << endl;

    //Load the training data to train the model
    RegressionData trainingData;
    RegressionData validationData;

    //Try and parse the input and target dimensions
    unsigned int numInputDimensions = 0;
    unsigned int numTargetDimensions = 0;
    if( parser.get("num-inputs",numInputDimensions) && parser.get("num-targets",numTargetDimensions) ){
      infoLog << "num input dimensions: " << numInputDimensions << " num target dimensions: " << numTargetDimensions << endl;
      trainingData.setInputAndTargetDimensions( numInputDimensions, numTargetDimensions );
    }

    if( (numInputDimensions == 0 || numTargetDimensions == 0) && Util::stringEndsWith( trainDatasetFilename, ".csv" ) ){
      errorLog << "Failed to parse num input dimensions and num target dimensions from input arguments. You must supply the input and target dimensions if the data format is CSV!" << endl;
      printHelp();
      return false;
    }

    infoLog << "loading training data..." << endl;
    if( !trainingData.load( trainDatasetFilename ) ){
        errorLog << "Failed to load training data!\n";
        return false;
    }

    const unsigned int N = trainingData.getNumInputDimensions();
    const unsigned int T = trainingData.getNumTargetDimensions();
    infoLog << "num training samples: " << trainingData.getNumSamples() << endl;
    infoLog << "num input dimensions: " << N << endl;
    infoLog << "num target dimensions: " << T << endl;
    
    if( validationDatasetFilename != "" ){
      infoLog << "loading validation data..." << endl;
      if( !validationData.load( validationDatasetFilename ) ){
        errorLog << "Failed to load validataion data!\n";
        return false;
      }
      infoLog << "num validation samples: " << validationData.getNumSamples() << endl;
    }

    //Create a new regression instance
    LogisticRegression regression;

    regression.setMaxNumEpochs( maxEpoch );
    regression.setMinChange( minChange );
    regression.setRandomiseTrainingOrder( true );
    regression.setUseValidationSet( true );
    regression.setValidationSetSize( 20 );
    regression.setBatchSize( batchSize );

    //Create a new pipeline that will hold the regression algorithm
    GestureRecognitionPipeline pipeline;

    //Add a multidimensional regression instance and set the regression algorithm to Linear Regression
    pipeline.setRegressifier( MultidimensionalRegression( regression, enableScaling ) );

    infoLog << "training model...\n";

    //Train the classifier
    if( validationData.getNumSamples() > 0 ){ 
      if( !pipeline.train( trainingData, validationData ) ){
        errorLog << "Failed to train model!" << endl;
        return false;
      }
    }else{
      if( !pipeline.train( trainingData ) ){
        errorLog << "Failed to train model!" << endl;
        return false;
      }
    }

    infoLog << "model trained!" << endl;

    infoLog << "saving model to: " << modelFilename << endl;

    //Save the pipeline
    if( pipeline.save( modelFilename ) ){
        infoLog << "model saved." << endl;
    }else warningLog << "Failed to save model to file: " << modelFilename << endl;

    infoLog << "trainingTime: " << pipeline.getTrainingTime() << endl;

    return true;
}

