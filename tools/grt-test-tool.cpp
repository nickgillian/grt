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
    infoLog << endl;
    return true;
}

bool test( CommandLineParser &parser );
bool saveResults( const GestureRecognitionPipeline &pipeline, const string &filename );

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
    parser.addOption( "-f", "dataset-filename" );
    parser.addOption( "-m", "model-filename" );
    parser.addOption( "-r", "results-filename" );

    //Parse the command line
    parser.parse( argc, argv );

    //Test the model
    if( test( parser ) ){
        infoLog << "Model Tested!" << endl;
        return EXIT_SUCCESS;
    }

    infoLog << "Failed to test model!" << endl;
    
    return EXIT_FAILURE;
}

bool test( CommandLineParser &parser ){

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
    parser.get("results-filename",resultsFilename,string("results.txt"));

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

    infoLog << "- Loading Training Data..." << endl;
    if( !data.load( datasetFilename ) ){
        errorLog << "Failed to load data!\n";
        return false;
    }

    const unsigned int N = data.getNumDimensions();
    infoLog << "- Num training samples: " << data.getNumSamples() << endl;
    infoLog << "- Num dimensions: " << N << endl;
    infoLog << "- Num classes: " << data.getNumClasses() << endl;

    //Test the classifier
    if( !pipeline.test( data ) ){
        errorLog << "Failed to test pipeline!" << endl;
        return false;
    }

    infoLog << "- Test complete in " << pipeline.getTestTime()/1000.0 << " seconds with and accuracy of: " << pipeline.getTestAccuracy() << endl;

    return saveResults( pipeline, resultsFilename );
}

bool saveResults( const GestureRecognitionPipeline &pipeline, const string &filename ){
    
    infoLog << "Saving results to file: " << filename << endl;

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

    file.close();

    infoLog << "Results saved." << endl;

    return true;
}

