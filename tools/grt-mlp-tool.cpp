/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for training a MLP neural net model. See the help string for more info.
*/

#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

const char *help = "This tool can be used for training a MLP Neural Net regression model.\n"
                   "The dataset used to train the model can be in two format, (1) a GRT RegressionData formatted file or (2) a CSV formatted file.\n"
                   "If the data is formatted as a CSV file, then it should be formatted as follows:\n"
                     "\t- each row should contain a sample\n"
                     "\t- the first N columns should contain the input attributes (a.k.a. features)\n"
                     "\t- the last K columns should contain the target attributes (a.k.a. targets or classes)\n"
                     "\t- columns should be seperated by a comma delimiter \',\'\n"
                     "\t- rows should be terminated by a new line operator \'\\n\'";
                                          
InfoLog infoLog("[grt-mlp-tool]");
WarningLog warningLog("[WARNING grt-mlp-tool]");
ErrorLog errorLog("[ERROR grt-mlp-tool]");

bool printHelp(){
  infoLog << "\nusage: grt-mlp-tool [options]\n";
  infoLog << "\t--help: prints this help message\n";
  infoLog << "\t-f: sets the filename the training data will be loaded from. The training data can either be a GRT RegressionData file or a CSV file (see help below). \n";
  infoLog << "\t--model: sets the filename the model will be saved to\n";
  infoLog << "\t--num-inputs: sets the number of inputs in the CSV file, required if the input data format is a CSV file\n";
  infoLog << "\t--num-targets: sets the number of targets in the CSV file, required if the input data format is a CSV file\n";
  infoLog << "\t--batch-size: sets the number of training samples used in each batch update of the learning algorithm (default: 50)\n";
  infoLog << "\t--learning-rate: sets the rate at which the learning algorithm will update the model weights at each batch update (default: 0.01)\n";
  infoLog << "\t--min-change: sets the minimum change needed to signal convergence of the learning algorithm (default: 0.001)\n";
  infoLog << "\t--max-epoch: sets the maximum number of epochs that the learning algorithm can run for, an epoch is one iteration over the entire training dataset (default: 500)\n";
  infoLog << "\t--log-file: \n";
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
    parser.addOption( "--model", "model-filename", "mlp-model.grt" ); //Set the default filename to log-regression-model.grt
    parser.addOption( "--num-inputs", "num-inputs" );
    parser.addOption( "--num-targets", "num-targets" );
    parser.addOption( "--num-hidden", "num-hidden" );
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
    bool enableScaling = true;
    float learningRate = 0;
    float minChange = 0;
    unsigned int maxEpoch = 0;
    unsigned int batchSize = 0;
    unsigned int numHiddenNeurons = 0;

    //Get the filename
    if( !parser.get("filename",trainDatasetFilename) ){
        errorLog << "Failed to parse filename from command line! You can set the filename using the -f." << endl;
        printHelp();
        return false;
    }
    
    //Get the parameters from the parser
    parser.get("model-filename",modelFilename);
    parser.get( "learning-rate", learningRate );
    parser.get( "min-change", minChange );
    parser.get( "max-epoch", maxEpoch );
    parser.get( "batch-size", batchSize );
    parser.get( "enable-scaling", enableScaling );
    parser.get( "num-hidden", numHiddenNeurons );

    infoLog << "settings: num hidden neurons: " << numHiddenNeurons << " learning-rate: " << learningRate << " min-change: " << minChange << " max-epoch: " << maxEpoch << " batch-size: " << batchSize << " scaling: " << enableScaling << endl;

    //Load the training data to train the model
    RegressionData trainingData;

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

    infoLog << "- Loading Training Data..." << endl;
    if( !trainingData.load( trainDatasetFilename ) ){
        errorLog << "Failed to load training data!\n";
        return false;
    }

    const unsigned int N = trainingData.getNumInputDimensions();
    const unsigned int T = trainingData.getNumTargetDimensions();
    infoLog << "- Num training samples: " << trainingData.getNumSamples() << endl;
    infoLog << "- Num input dimensions: " << N << endl;
    infoLog << "- Num target dimensions: " << T << endl;

    //Create a new regression instance
    MLP regression;

    regression.setMaxNumEpochs( maxEpoch );
    regression.setMinChange( minChange );
    regression.setUseValidationSet( true );
    regression.setValidationSetSize( 20 );
    regression.setRandomiseTrainingOrder( true );
    regression.setNumRestarts( 1 );
    regression.enableScaling( enableScaling );
    regression.init( N, numHiddenNeurons, T, Neuron::LINEAR, Neuron::TANH, Neuron::TANH );

    //Create a new pipeline that will hold the regression algorithm
    GestureRecognitionPipeline pipeline;

    //Add a multidimensional regression instance and set the regression algorithm to Linear Regression
    //pipeline.setRegressifier( MultidimensionalRegression( regression, true ) );
    pipeline << regression;

    infoLog << "- Training model...\n";

    //Train the MLP model
    if( !pipeline.train_( trainingData ) ){
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

