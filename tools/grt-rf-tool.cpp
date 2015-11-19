/**
 @author Nicholas Gillian <nick@nickgillian.com>
 @brief This file implements a basic tool for processing data files and training a random forest model.
*/

#include <GRT/GRT.h>
using namespace GRT;

InfoLog infoLog("[grt-rf-tool]");
WarningLog warningLog("[WARNING grt-rf-tool]");
ErrorLog errorLog("[ERROR grt-rf-tool]");

bool printUsage(){
    infoLog << "grt-rf-tool [options]\n";
    infoLog << "\t-m: sets the current application mode, can be: \n";
    infoLog << "\t\t[1] 'train-model': trains a random forest model, using a pre-recorded training dataset.\n";
    infoLog << "\t\t[2] 'combine-models': combines multiple random forest models into a single random forest model.\n";
    infoLog << "\t\t[3] 'compute-weights': computes the feature weights for the random forest model and saves it to a file.\n";
    infoLog << "\t-f: sets the main filename. \n";
    infoLog << "\t\tIn 'train-model' mode, this sets the name of the file the training data will be loaded from.\n";
    infoLog << "\t\tIn 'combine-models' mode this sets the name of the file the combined model will be saved to.\n";
    infoLog << "\t\tIn 'compute-weights' mode this sets the name of the file the feature weights results will be saved to.\n";
    infoLog << "\t-d: sets the data directory containing the raw csv files that will be combined into the main dataset, only needed for 'combine-files' mode\n";
    infoLog << "\t\tIn 'combine-models' mode, this sets the data directory containing the random forest model files that will be combined into the main random forest model.\n";
    infoLog << "\t--model: sets the filename the random forests model will be saved to\n";
    infoLog << "\t--forest-size: sets the number of tress in the forest, only used for 'train-model'\n";
    infoLog << "\t--max-depth: sets the maximum depth the forest can reach, only used for 'train-model'\n";
    infoLog << "\t--min-node-size: sets the minimum number of training samples allowed per node, only used for 'train-model'\n";
    infoLog << "\t--num-splits: sets the number of random splits allowed per node, only used for 'train-model'\n";
    infoLog << "\t--remove-features: sets if features should be removed at each split [1=true,0=false], only used for 'train-model'\n";
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
    parser.addOption( "-m", "mode" );
    parser.addOption( "-f", "filename" );
    parser.addOption( "-d", "data-dir" );
    parser.addOption( "--model", "model-filename" );
    parser.addOption( "--results", "results-filename" );
    parser.addOption( "--forest-size", "forest-size" );
    parser.addOption( "--max-depth", "max-depth" );
    parser.addOption( "--min-node-size", "min-node-size" );
    parser.addOption( "--num-splits", "num-splits" );
    parser.addOption( "--remove-features", "remove-features" );

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

    //Combine models mode
    if( mode == "combine-models" ){
        if( combineModels( parser ) ){
            infoLog << "Models combined." << endl;
            return EXIT_SUCCESS;
        }else{
            errorLog << "Failed to combine models!" << endl;
            return EXIT_FAILURE;
        }
    }

    //Compute feature weights
    if( mode == "compute-weights" ){
        if( computeFeatureWeights( parser ) ){
            infoLog << "Feature Weights Computed" << endl;
            return EXIT_SUCCESS;
        }else{
            errorLog << "Failed to compute feature weights!" << endl;
            return EXIT_FAILURE;
        }
    }

    errorLog << "Unknown application mode: " << mode << endl;
    printUsage();

    return EXIT_FAILURE;
}

bool train( CommandLineParser &parser ){

    infoLog << "Training Random Forest model..." << endl;

    string trainDatasetFilename = "";
    string modelFilename = "";
    string defaultFilename = "rf-model.grt";
    unsigned int numThreads = 0;
    unsigned int forestSize = 0;
    unsigned int maxDepth = 0;
    unsigned int minNodeSize = 0;
    unsigned int numSplits = 0;
    unsigned int defaultForestSize = 5;
    unsigned int defaultMaxDepth = 10;
    unsigned int defaultMinNodeSize = 50;
    unsigned int defaultNumSplits = 100;
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

    //Get the forest size
    parser.get("forest-size",forestSize,defaultForestSize);

    //Get the max depth
    parser.get("max-depth",maxDepth,defaultMaxDepth);

    //Get the min node size
    parser.get("min-node-size",minNodeSize,defaultMinNodeSize);

    //Get the number of random splits
    parser.get("num-splits",numSplits,defaultNumSplits);
    
    //Get the remove features
    parser.get("remove-features",removeFeatures,defaultRemoveFeatures);

    //Load some training data to train the classifier
    ClassificationData trainingData;

    infoLog << "- Loading Training Data..." << endl;
    if( !trainingData.load( trainDatasetFilename ) ){
        errorLog << "Failed to load training data!\n";
        return false;
    }

    const unsigned int N = trainingData.getNumDimensions();
    std::vector< ClassTracker > tracker = trainingData.getClassTracker();
    infoLog << "- Num training samples: " << trainingData.getNumSamples() << endl;
    infoLog << "- Num dimensions: " << N << endl;
    infoLog << "- Num classes: " << trainingData.getNumClasses() << endl;
    infoLog << "- Class stats: " << endl;
    for(size_t i=0; i<tracker.size(); i++){
        infoLog << "- class " << tracker[i].classLabel << " number of samples: " << tracker[i].counter << endl;
    }
    
    //Create a new RandomForests instance
    RandomForests forest;

    //Set the decision tree node that will be used for each tree in the forest
    forest.setDecisionTreeNode( DecisionTreeClusterNode() );

    //Set the number of trees in the forest
    forest.setForestSize( forestSize );

    //Set the maximum depth of the tree
    forest.setMaxDepth( maxDepth );

    //Set the minimum number of samples allowed per node
    forest.setMinNumSamplesPerNode( minNodeSize );

    //Set the number of random splits used per node
    forest.setNumRandomSplits( numSplits );

    //Set if selected features should be removed at each node
    forest.setRemoveFeaturesAtEachSpilt( removeFeatures );

    //Add the classifier to a pipeline
    GestureRecognitionPipeline pipeline;
    pipeline.setClassifier( forest );

    infoLog << "- Training model...\n";

    //Train the classifier
    if( !pipeline.train( trainingData ) ){
        errorLog << "Failed to train classifier!" << endl;
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

bool combineModels( CommandLineParser &parser ){

    infoLog << "Combining models..." << endl;

    string directoryPath = "";
    string modelFilename = "";

    if( !parser.get("data-dir",directoryPath) ){
        errorLog << "Failed to parse data-directory from command line! You can set the data-directory using the --data-dir option." << endl;
        printUsage();
        return false;
    }

    //Get the filename
    if( !parser.get("model-filename",modelFilename) ){
        errorLog << "Failed to parse filename from command line! You can set the model filename using the --model." << endl;
        printUsage();
        return false;
    }

    vector< string > files;

    infoLog << "- Parsing data directory: " << directoryPath << endl;

    //Parse the directory to get all the csv files
    if( !Util::parseDirectory( directoryPath, ".grt", files ) ){
        errorLog << "Failed to parse data directory!" << endl;
        return false;
    }

    RandomForests forest; //Used to validate the random forest type
    GestureRecognitionPipeline *mainPipeline = NULL; // Points to the first valid pipeline that all the models will be merged to
    std::vector< GestureRecognitionPipeline* > pipelineBuffer; //Stores the pipeline for each file that is loaded
    unsigned int inputVectorSize = 0; //Set to zero to mark we haven't loaded any models yet
    const unsigned int numFiles = (unsigned int)files.size();
    bool mainPipelineSet = false;
    bool combineModelsSuccessful = false;

    pipelineBuffer.reserve( numFiles );
    
    //Loop over the files, load them, and add valid random forest pipelines to the pipelineBuffer so they can be combined with the mainPipeline
    for(unsigned int i=0; i<numFiles; i++){
        infoLog << "- Loading model " << files[i] << ". File " << i+1 << " of " << numFiles << endl;

        GestureRecognitionPipeline *pipeline = new GestureRecognitionPipeline;

        if( pipeline->load( files[i] ) ){

            infoLog << "- Pipeline loaded. Number of input dimensions: " << pipeline->getInputVectorDimensionsSize() << endl;

            if( pipelineBuffer.size() == 0 ){
                inputVectorSize = pipeline->getInputVectorDimensionsSize();
            }

            if( pipeline->getInputVectorDimensionsSize() != inputVectorSize ){
                warningLog << "- Pipeline " << i+1 << " input vector size does not match the size of the first pipeline!" << endl;
            }else{

                Classifier *classifier = pipeline->getClassifier();
                if( classifier ){
                    if( classifier->getClassifierType() == forest.getClassifierType() ){ //Validate the classifier is a random forest
                        if( !mainPipelineSet ){
                            mainPipelineSet = true;
                            mainPipeline = pipeline;
                        }else pipelineBuffer.push_back( pipeline );
                    }else{
                        warningLog << "- Pipeline " << i+1 << " does not contain a random forest classifer! Classifier type: " << classifier->getClassifierType() << endl;
                    }
                }

            }
        }else{
            warningLog << "- WARNING: Failed to load model from file: " << files[i] << endl;
        }
    }

    if( mainPipelineSet ){

        //Combine the random forest models with the main pipeline model
        const unsigned int numPipelines = (unsigned int)pipelineBuffer.size();
        RandomForests *mainForest = mainPipeline->getClassifier< RandomForests >();

        for(unsigned int i=0; i<numPipelines; i++){

            infoLog << "- Combing model " << i+1 << " of " << numPipelines << " with main model..." << endl;

            RandomForests *f = pipelineBuffer[i]->getClassifier< RandomForests >();

            if( !mainForest->combineModels( *f ) ){
                warningLog << "- WARNING: Failed to combine model " << i+1 << " with the main model!" << endl;
            }
        }

        if( mainPipeline->getTrained() ){
            infoLog << "- Saving combined pipeline to file..." << endl;
            combineModelsSuccessful = mainPipeline->save( modelFilename );
        }

    }else{
        errorLog << "Failed to combined models, no models were loaded!" << endl;
    }

    //Cleanup the pipeline buffer
    for(size_t i=0; i<pipelineBuffer.size(); i++){
        delete pipelineBuffer[i];
        pipelineBuffer[i] = NULL;
    }

    return combineModelsSuccessful;
}

bool computeFeatureWeights( CommandLineParser &parser ){

    infoLog << "Computing feature weights..." << endl;

    string resultsFilename = "";
    string modelFilename = "";

    //Get the model filename
    if( !parser.get("model-filename",modelFilename) ){
        errorLog << "Failed to parse filename from command line! You can set the model filename using the --model." << endl;
        printUsage();
        return false;
    }

    //Get the results filename
    if( !parser.get("filename",resultsFilename) ){
        errorLog << "Failed to parse results filename from command line! You can set the results filename using the -f." << endl;
        printUsage();
        return false;
    }

    //Load the model
    GestureRecognitionPipeline pipeline;

    if( !pipeline.load( modelFilename ) ){
        errorLog << "Failed to load model from file: " << modelFilename << endl;
        printUsage();
        return false;
    }

    //Make sure the pipeline contains a random forest model and that it is trained
    RandomForests *forest = pipeline.getClassifier< RandomForests >();

    if( !forest ){
        errorLog << "Model loaded, but the pipeline does not contain a RandomForests classifier!" << endl;
        printUsage();
        return false;
    }

    if( !forest->getTrained() ){
        errorLog << "Model loaded, but the RandomForests classifier is not trained!" << endl;
        printUsage();
        return false;
    }
/*
    //Compute the feature weights
    VectorDouble weights = forest->getFeatureWeights();
    if( weights.size() == 0 ){
        errorLog << "Failed to compute feature weights!" << endl;
        printUsage();
        return false;
    }

    //Save the results to a file
    fstream file;
    file.open( resultsFilename.c_str(), fstream::out );
    
    const size_t N = weights.size();
    for(unsigned int i=0; i<N; i++){
        file << weights[i] << endl;
    }
    
    file.close();
*/
    return true;
}


