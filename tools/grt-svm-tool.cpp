/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for processing data files and training a random forest model.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

InfoLog infoLog("[grt-svm-tool]");
WarningLog warningLog("[WARNING grt-svm-tool]");
ErrorLog errorLog("[ERROR grt-svm-tool]");

bool printUsage(){
    infoLog << "grt-svm-tool [options]\n";
    infoLog << "\t-f: sets the name of the file the training data will be loaded from.\n";
    infoLog << "\t--model: sets the filename the random forests model will be saved to\n";
    infoLog << "\t--forest-size: sets the number of tress in the forest, only used for 'train-model'\n";
    infoLog << "\t--max-depth: sets the maximum depth the forest can reach, only used for 'train-model'\n";
    infoLog << "\t--min-node-size: sets the minimum number of training samples allowed per node, only used for 'train-model'\n";
    infoLog << "\t--num-splits: sets the number of random splits allowed per node, only used for 'train-model'\n";
    infoLog << "\t--remove-features: sets if features should be removed at each split [1=true,0=false], only used for 'train-model'\n";
    infoLog << "\t--bootstrap-weight: sets the size of the dataset used to train each tree in the RF model, only used for 'train-model'\n";
    infoLog << "\t--combine-weights: 1/0 if true, then the random forest weights will be combined across all trees in the forest, only used for 'compute-weights' mode\n";
    infoLog << endl;
    return true;
}

bool train( CommandLineParser &parser );
bool combineModels( CommandLineParser &parser );
bool computeFeatureWeights( CommandLineParser &parser );

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
    parser.addOption( "-f", "filename" );
    parser.addOption( "--model", "model-filename" );
    parser.addOption( "--results", "results-filename" );
    parser.addOption( "--kernel-type", "kernel-type" );

    //Parse the command line
    parser.parse( argc, argv );

    string filename = "";

    //Train SVM model
    if( train( parser ) ){
        infoLog << "Model Trained!" << endl;
        return EXIT_SUCCESS;
    }

    printUsage();

    return EXIT_FAILURE;
}

bool train( CommandLineParser &parser ){

    string trainDatasetFilename = "";
    string modelFilename = "";
    string defaultFilename = "svm-model.grt";
    string kernelTypeStr = "";
    SVM::KernelType kernelType;

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

    if( !parser.get("kernel-type",kernelTypeStr) ){
        errorLog << "Failed to parse kernel type from command line! You can set the kernel type using the --kernel-type." << endl;
        return false;
    }

    //Conver the kernel type string to the enum
    if( kernelTypeStr == "" || kernelTypeStr == "linear" ){
        kernelType = SVM::LINEAR_KERNEL;
    }else if( kernelTypeStr == "poly" ){
         kernelType = SVM::POLY_KERNEL;
    }else if( kernelTypeStr == "rbf" ){
         kernelType = SVM::RBF_KERNEL;
    }else if( kernelTypeStr == "sigmoid" ){
         kernelType = SVM::SIGMOID_KERNEL;
    }else{
        errorLog << "Unknown kernel-type: " << kernelTypeStr << endl;
        return false;
    }

    //Load some training data to train the classifier
    ClassificationData trainingData;

    infoLog << "- Loading Training Data..." << endl;
    if( !trainingData.load( trainDatasetFilename ) ){
        errorLog << "Failed to load training data!\n";
        return false;
    }

    const unsigned int N = trainingData.getNumDimensions();
    Vector< ClassTracker > tracker = trainingData.getClassTracker();
    infoLog << "- Num training samples: " << trainingData.getNumSamples() << endl;
    infoLog << "- Num dimensions: " << N << endl;
    infoLog << "- Num classes: " << trainingData.getNumClasses() << endl;
    infoLog << "- Class stats: " << endl;
    for(unsigned int i=0; i<tracker.getSize(); i++){
        infoLog << "- class " << tracker[i].classLabel << " number of samples: " << tracker[i].counter << endl;
    }
    
    GRT::SVM svm( kernelType );
    

    //Add the classifier to a pipeline
    GestureRecognitionPipeline pipeline;
    pipeline << svm;

    infoLog << "- Training SVM model..." << endl;

    //Train the classifier
    if( !pipeline.train( trainingData ) ){
        errorLog << "Failed to train classifier!" << endl;
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

    return true;
}
