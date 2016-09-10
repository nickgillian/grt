/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for spliting data and saving the new data to two files.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

InfoLog infoLog("[grt-split-tool]");
WarningLog warningLog("[WARNING grt-split-tool]");
ErrorLog errorLog("[ERROR grt-split-tool]");

bool printUsage(){
    infoLog << "grt-split-tool INPUT_FILENAME OUT_FILENAME_1 OUT_FILENAME_2" << endl;
    return true;
}

int main(int argc, char * argv[])
{

    if( argc < 4 ){
        errorLog << "Not enough input arguments!" << endl;
        printUsage();
        return EXIT_FAILURE;
    }

    const string inputFilename = argv[1];
    const string outputFilename1 = argv[2];
    const string outputFilename2 = argv[3];
    unsigned int splitPercentage = 80;
    bool useStratifiedSampling = true;

    //Create an instance of the parser
    CommandLineParser parser;

    //Disable warning messages
    parser.setWarningLoggingEnabled( false );

    //Add some options and identifiers that can be used to get the results
    parser.addOption( "--split-perc", "split-percentage", 80 ); //Set the default split percentage to 80% (80% train, 20% test)
    parser.addOption( "--use-strat", "stratified-sampling", true );

    //Parse the command line
    parser.parse( argc, argv );

    //Get the options
    parser.get( "split-percentage", splitPercentage );
    parser.get( "stratified-sampling", useStratifiedSampling );

    infoLog << "- Loading main dataset: " << inputFilename << endl;

    ClassificationData data;
  
    //Load the raw data
    if( !data.load( inputFilename ) ){
        errorLog << "Failed to load input data: " << inputFilename << endl;
        return EXIT_FAILURE; 
    }

    infoLog << "- Loaded dataset with " << data.getNumSamples() << " samples" << endl;

    ClassificationData subset = data.split( splitPercentage, useStratifiedSampling );

    infoLog << "- Data split." << endl;
    infoLog << "- Main Dataset. Number of samples: " << data.getNumSamples() << endl;
    infoLog << "- Subset. Number of samples: " << subset.getNumSamples() << endl;

    //Save the new datasets
    infoLog << "- Saving main dataset to file: " << outputFilename1 << endl;
    if( !data.save( outputFilename1 ) ){
        errorLog << "Failed to save output data: " << outputFilename1 << endl;
        return EXIT_FAILURE; 
    }

    infoLog << "- Saving subset to file: " << outputFilename2 << endl;
    if( !subset.save( outputFilename2 ) ){
        errorLog << "Failed to save output data: " << outputFilename2 << endl;
        return EXIT_FAILURE; 
    }
    
    return EXIT_SUCCESS;
}

