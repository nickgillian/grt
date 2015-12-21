/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for processing data files and training a linear regression model.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

InfoLog infoLog("[grt-lin-reg-tool]");
WarningLog warningLog("[WARNING grt-lin-reg-tool]");
ErrorLog errorLog("[ERROR grt-lin-reg-tool]");

bool printUsage(){
    infoLog << "grt-lin-reg-tool [options]\n";
    infoLog << "\t-m: sets the current application mode, can be: \n";
    infoLog << "\t\t[1] 'train-model': trains a linear regression model, using a pre-recorded training dataset.\n";
    infoLog << "\t-f: sets the main filename. \n";
    infoLog << "\t\tIn 'train-model' mode, this sets the name of the file the training data will be loaded from.\n";
    infoLog << "\t--model: sets the filename the regression model will be saved to\n";
    infoLog << endl;
    return true;
}

bool train( CommandLineParser &parser );

int main(int argc, char * argv[])
{

    if( argc < 2 ){
        errorLog << "Not enough input arguments!" << endl;
        printUsage();
        return EXIT_FAILURE;
    }

    //Create an instance of the parser
    CommandLineParser parser;

    //Disable warning messages
    parser.setWarningLoggingEnabled( false );

    //Add some options and identifiers that can be used to get the results
    parser.addOption( "-m", "mode" );
    parser.addOption( "-f", "filename" );
    parser.addOption( "--model", "model-filename" );

    //Parse the command line
    parser.parse( argc, argv );

    string mode = "";
    string filename = "";

    //Get the application mode
    if( !parser.get("mode",mode) ){
        errorLog << "Failed to parse mode from command line! You can set the mode using the -m option." << endl;
        printUsage();
        return EXIT_FAILURE;
    }

    //Train RF model
    if( mode == "train-model" ){
        if( train( parser ) ){
            infoLog << "Model Trained!" << endl;
            return EXIT_SUCCESS;
        }
        return EXIT_FAILURE;
    }

    errorLog << "Unknown application mode: " << mode << endl;
    printUsage();

    return EXIT_FAILURE;
}

bool train( CommandLineParser &parser ){

    infoLog << "Training regression model..." << endl;

    string trainDatasetFilename = "";
    string modelFilename = "";
    string defaultFilename = "linear-regression-model.grt";
    bool removeFeatures = false;
    bool defaultRemoveFeatures = false;

    //Get the filename
    if( !parser.get("filename",trainDatasetFilename) ){
        errorLog << "Failed to parse filename from command line! You can set the filename using the -f." << endl;
        printUsage();
        return false;
    }

    //Get the model filename
    parser.get("model-filename",modelFilename,defaultFilename);

    //Load the training data to train the model
    RegressionData trainingData;

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
    LinearRegression regression;

    regression.setMaxNumEpochs( 500 );
    regression.setMinChange( 1.0e-5 );
    regression.setUseValidationSet( true );
    regression.setValidationSetSize( 20 );
    regression.setRandomiseTrainingOrder( true );
    regression.enableScaling( true );

    //Create a new pipeline that will hold the regression algorithm
    GestureRecognitionPipeline pipeline;

    //Add a multidimensional regression instance and set the regression algorithm to Linear Regression
    pipeline.setRegressifier( MultidimensionalRegression( regression, true ) );

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

