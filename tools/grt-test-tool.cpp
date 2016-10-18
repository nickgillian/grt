/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for testing classification models.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

InfoLog infoLog("[grt-test-tool]");
WarningLog warningLog("[WARNING grt-test-tool]");
ErrorLog errorLog("[ERROR grt-test-tool]");

bool printUsage(){
    infoLog << "grt-test-tool [options]\n";
    infoLog << "\t-f: sets the name of the file the test dataset should be loaded from \n";
    infoLog << "\t-m: sets the name of the file the test model should be loaded from\n";
    infoLog << "\t-r: sets the name of the file the results will be saved to\n";
    infoLog << "\t--all: outputs the prediction for each test sample to the results file\n";
    infoLog << endl;
    return true;
}

enum Mode{
  CLASSIFICATION_MODE=0,
  REGRESSION_MODE
};

bool test_classification( CommandLineParser &parser );
bool test_regression( CommandLineParser &parser );
bool saveResults( const GestureRecognitionPipeline &pipeline, const string &filename, const bool exportTimeseries );

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
    parser.addOption( "-f", "dataset-filename" ); //This should be supplied by the user as an input argument
    parser.addOption( "-m", "model-filename", "model.grt" );
    parser.addOption( "-r", "results-filename", "results.txt" );
    parser.addOption( "--mode", "mode", CLASSIFICATION_MODE );
    parser.addOption( "--export-timeseries", "export-timeseries", false );

    //Parse the command line
    parser.parse( argc, argv );

    //Get the mode
    Mode mode = CLASSIFICATION_MODE;

    switch( mode ){
      case CLASSIFICATION_MODE:
          if( test_classification( parser ) ){
            return EXIT_SUCCESS;
          }
        break;
      case REGRESSION_MODE:
          if( test_regression( parser ) ){
            return EXIT_SUCCESS;
          }
        break;
      default:
        errorLog << "Unkown mode!" << endl;
        return EXIT_FAILURE;
        break;
    }
 
    return EXIT_FAILURE;
}

bool test_classification( CommandLineParser &parser ){

    infoLog << "Testing model..." << endl;

    string datasetFilename = "";
    string modelFilename = "";
    string resultsFilename = "";
    bool exportTimeseries = false;

    //Get the model filename
    if( !parser.get("model-filename",modelFilename) ){
        errorLog << "Failed to parse model filename from command line! You can set the model filename using the -m." << endl;
        printUsage();
        return false;
    }

    //Get the filename
    if( !parser.get("dataset-filename",datasetFilename) ){
        errorLog << "Failed to parse dataset filename from command line! You can set the dataset filename using the -f." << endl;
        printUsage();
        return false;
    }

    //Get the model filename
    if( !parser.get("results-filename",resultsFilename) ){
        errorLog << "Failed to parse results filename from command line! You can set the results filename using the -r." << endl;
        printUsage();
        return false; 
    }

    //Get the export all parameter
    parser.get("export-timeseries",exportTimeseries);

    //Load the pipeline from a file
    GestureRecognitionPipeline pipeline;

    infoLog << "- Loading model..." << endl;

    if( !pipeline.load( modelFilename ) ){
        errorLog << "Failed to load model from file: " << modelFilename << endl;
        printUsage();
        return false;
    }

    infoLog << "- Model loaded!" << endl;

    //Load the data to test the classifier
    ClassificationData data;

    infoLog << "- Loading test dataset..." << endl;
    if( !data.load( datasetFilename ) ){
        errorLog << "Failed to load data!\n";
        return false;
    }

    const unsigned int N = data.getNumDimensions();
    infoLog << "- Num test samples: " << data.getNumSamples() << endl;
    infoLog << "- Num dimensions: " << N << endl;
    infoLog << "- Num classes: " << data.getNumClasses() << endl;

    //Test the classifier
    if( !pipeline.test( data ) ){
        errorLog << "Failed to test pipeline!" << endl;
        return false;
    }

    infoLog << "- Test complete in " << pipeline.getTestTime()/1000.0 << " seconds with and accuracy of: " << pipeline.getTestAccuracy() << endl;

    return saveResults( pipeline, resultsFilename, exportTimeseries );
}

bool test_regression( CommandLineParser &parser ){

    infoLog << "Testing model..." << endl;

    string datasetFilename = "";
    string modelFilename = "";
    string resultsFilename = "";

    //Get the model filename
    if( !parser.get("model-filename",modelFilename) ){
        errorLog << "Failed to parse model filename from command line! You can set the model filename using the -m." << endl;
        printUsage();
        return false;
    }

    //Get the filename
    if( !parser.get("dataset-filename",datasetFilename) ){
        errorLog << "Failed to parse dataset filename from command line! You can set the dataset filename using the -f." << endl;
        printUsage();
        return false;
    }

    //Get the model filename
    parser.get("results-filename",resultsFilename);

    //Load the pipeline from a file
    GestureRecognitionPipeline pipeline;

    infoLog << "- Loading model..." << endl;

    if( !pipeline.load( modelFilename ) ){
        errorLog << "Failed to load model from file: " << modelFilename << endl;
        printUsage();
        return false;
    }

    infoLog << "- Model loaded!" << endl;

    //Load the data to test the model
    RegressionData data;

    infoLog << "- Loading test dataset..." << endl;
    if( !data.load( datasetFilename ) ){
        errorLog << "Failed to load data!\n";
        return false;
    }

    const unsigned int N = data.getNumInputDimensions();
    const unsigned int T = data.getNumTargetDimensions();
    infoLog << "- Num test samples: " << data.getNumSamples() << endl;
    infoLog << "- Num input dimensions: " << N << endl;
    infoLog << "- Num target dimensions: " << T << endl;

    //Test the classifier
    if( !pipeline.test( data ) ){
        errorLog << "Failed to test pipeline!" << endl;
        return false;
    }

    infoLog << "- Test complete in " << pipeline.getTestTime()/1000.0 << " seconds with and accuracy of: " << pipeline.getTestAccuracy() << endl;

    return saveResults( pipeline, resultsFilename, false );
}

bool saveResults( const GestureRecognitionPipeline &pipeline, const string &filename, const bool exportTimeseries ){
    
    infoLog << "Saving results to file: " << filename << endl;
    infoLog << "Exporting timeseries: " << exportTimeseries << endl;

    fstream file( filename.c_str(), fstream::out );

    if( !file.is_open() ){
        errorLog << "Failed to open results file: " << filename << endl;
        return false;
    }

    file << pipeline.getTestAccuracy() << endl;

    Vector< UINT > classLabels = pipeline.getClassLabels();

    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        file << pipeline.getTestPrecision( classLabels[k] );
        if( k+1 < pipeline.getNumClassesInModel() ) file << "\t";
        else file << endl;
    }

    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        file << pipeline.getTestRecall( classLabels[k] );
        if( k+1 < pipeline.getNumClassesInModel() ) file << "\t";
        else file << endl;
    }

    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        file << pipeline.getTestFMeasure( classLabels[k] );
        if( k+1 < pipeline.getNumClassesInModel() ) file << "\t";
        else file << endl;
    }

    MatrixFloat confusionMatrix = pipeline.getTestConfusionMatrix();
    for(UINT i=0; i<confusionMatrix.getNumRows(); i++){
        for(UINT j=0; j<confusionMatrix.getNumCols(); j++){
            file << confusionMatrix[i][j];
            if( j+1 < confusionMatrix.getNumCols() ) file << "\t";
        }file << endl;
    }

    if( exportTimeseries ){
        Vector< TestInstanceResult > testInstanceResults = pipeline.getTestInstanceResults();
        VectorFloat classLikelihoods;
        for(UINT i=0; i<testInstanceResults.getSize(); i++){
            const TestInstanceResult &result = testInstanceResults[i];
            switch( result.getTestMode() ){
                case TestInstanceResult::CLASSIFICATION_MODE:
                {
                    file << result.getClassLabel() << "\t";
                    file << result.getPredictedClassLabel() << "\t";
                    classLikelihoods = result.getClassLikelihoods();
                    for(UINT k=0; k<classLikelihoods.getSize(); k++){
                        file << classLikelihoods[k];
                        if( k+1 < classLikelihoods.getSize() ) file << "\t";
                    }
                    file << endl;
                }
                break;
                case TestInstanceResult::REGRESSION_MODE:
                    //Todo
                break;
            }
        }
    }

    file.close();

    infoLog << "Results saved." << endl;

    return true;
}

