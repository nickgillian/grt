/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for merging multiple data files into one dataset and saving the new dataset to a file.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

InfoLog infoLog("[grt-merge-tool]");
WarningLog warningLog("[WARNING grt-merge-tool]");
ErrorLog errorLog("[ERROR grt-merge-tool]");

bool printUsage(){
    infoLog << "grt-merge-tool INPUT_DIRECTORY OUT_FILENAME" << endl;
    return true;
}

int main(int argc, char * argv[])
{

    if( argc < 3 ){
        errorLog << "Not enough input arguments!" << endl;
        printUsage();
        return EXIT_FAILURE;
    }

    const string inputDirectory = argv[1];
    const string outputFilename = argv[2];

    //Parse the data directory for files
    Vector< string > filenames;
    infoLog << "- Parsing data directory: " << inputDirectory << endl;

    if( !Util::parseDirectory( inputDirectory, ".csv", filenames ) ){
        errorLog << "Failed to parse input directory: " << inputDirectory << endl;
        return EXIT_FAILURE; 
    }

    if( filenames.getSize() == 0 ){
        errorLog << "Failed to find any files in the input directory: " << inputDirectory << endl;
        return EXIT_FAILURE; 
    }

    ClassificationData data;
    unsigned int numFiles = filenames.getSize();
    bool dataLoaded = false;
    for(unsigned int i=0; i<numFiles; i++){
        //Load the data
        infoLog << "- Loading data " << i+1 << " of " << numFiles << endl;
        
        ClassificationData tmp;
        if( tmp.load( filenames[i] ) ){
            if( i==0 ){
                data.setNumDimensions( tmp.getNumDimensions() );
            }
            dataLoaded = true;

            infoLog << "- Data loaded.  Number of samples: " << tmp.getNumSamples() << endl;

            data.merge( tmp );
        }else{
            warningLog << "- Failed to load data!" << endl;
        }
    }

    if( dataLoaded ){

        infoLog << "- Merged data to generate new dataset with " << data.getNumSamples() << " samples" << endl;

        //Save the new datasets
        infoLog << "- Saving main dataset to file: " << outputFilename << endl;
        if( !data.save( outputFilename ) ){
            errorLog << "Failed to save output data: " << outputFilename << endl;
            return EXIT_FAILURE; 
        }

    }else{
        warningLog << "- Failed to load any data!" << endl;
        return EXIT_FAILURE; 
    } 

    return EXIT_SUCCESS;
}

