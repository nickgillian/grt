/*
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 Regressifier Example
 
 This example shows you how to:
 - setup a gesture recognition pipeline
 - record your own dataset and save it to a file
 - load the dataset back from a file
 - train an Artificial Neural Network (ANN) regression algorithm using the training dataset
 - use the trained ANN algorithm to predict the output for real-time data input
 
 This example uses the 2-dimensional [x y] coordinates from your mouse as input, but you can easily change this to whatever sensor input
 you have access to. The 2-dimensional input vector is then mapped to a 1-dimensional output vector (called a target vector). The ANN algorithm
 can map an N-dimensional signal to an M-dimensional signal, so you are not limited to just using a 1-dimensional target vector.
 
 To compile this example:
 - use the Openframeworks project builder to create a new project
 - when you have created the new project, override the default testApp.h, testApp.cpp, and main.cpp files with the files from this example
 - open the project in your favorite IDE (XCode, Visual Studio, Code Blocks, etc.) and add the main GRT source folder to the project. You
   can find the main GRT source folder by looking for the folder called GRT in the directory you downloaded from google code. Most IDE's let
   you just drag and drop the entire GRT code folder into your project.
 - note that some IDE's make you specify the location of the GRT source code folder (for example Visual Studio). To do this, open the project's
   properties or setting pane and add the path to the GRT folder to your project's cpp Include section. In XCode you can just drag and drop the
   GRT folder directly from finder into your project.
 - compile openframeworks
 - compile this project
 
 When you have compiled this project, this is how you use it:
 - run the project
 - when you start the project, you will have no training data and the regressifier will not be trained so you need to do three things: 
   (1) record some training data
   (2) train your pipeline
   (3) use the pipeline to predict the output of real-time data
 - Step 1:
   - to record some training data, first make sure the value beside the TargetVector is set to the you want to record
   - to change the target vector you can use the '[' and ']' keys, [ to decrease the value and ] to increase the value
   - place your mouse in a specific region of the screen (for instance the top left corner)
   - press the 'r' key to start recording the training data
   - move your mouse around that area of the screen and record a few seconds of data
   - press the 'r' key to stop recording the training data
   - change the target vector to a new value and move your mouse to another section of the screen (for instance the bottom right corner)
   - press the 'r' key to start the recording, move your mouse around that section of the screen for a few seconds, stop the recording
   - keep repeating these steps until you have recorded all the training data you want
   - when you have finished, press the 's' key to save the training data to a file
   - if you need to load the training data at a later stage, for instance when you next restart the program, press the 'l' key
 - Step 2:
   - after you have recorded your training data, you can now train your pipeline
   - to train your pipeline, press the 't' key
   - if the pipeline trained a regression model successfully then you will see the info message: Pipeline Trained, otherwise you will see the 
     warning message WARNING: Failed to train pipeline. If the training failed, then make sure you have successfully recorded the training data 
 - Step 3:
   - after you have trained the pipeline, you can now use the pipeline to predict the output for real-time input data
   - if the pipeline was trained, it will automatically start to map the input data to the predicted output data
   - move your mouse around the screen and you should see the predicted output vector change through the values you trained the model to predict
 */


#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //Initialize the training and info variables
    infoText = "";
    record = false;
    targetVector.resize(1);
    targetVector[0] = 1;
    
    //The input to the training data will be the [x y] from the mouse, so we set the number of dimensions to 2
    //For regression data will also need to set the number of target dimensions, this will be set to 1
    trainingData.setInputAndTargetDimensions(2, 1);
    
    //Setup the MLP, the number of input and output neurons must match the dimensionality of the training/test datasets
    MLP mlp;
    UINT numInputNeurons = trainingData.getNumInputDimensions();
    UINT numHiddenNeurons = 2;
    UINT numOutputNeurons = trainingData.getNumTargetDimensions();
    
    //Initialize the MLP
    mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons);
    
    //Set the training settings
    mlp.setMaxNumEpochs( 100 ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
    mlp.setMinChange( 1.0e-2 ); //This sets the minimum change allowed in training error between any two epochs
    mlp.setNumRandomTrainingIterations( 5 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
    mlp.setUseValidationSet( true ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
    mlp.setValidationSetSize( 20 ); //Use 20% of the training data for validation during the training phase
    mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
    
    //The MLP generally works much better if the training and prediction data is first scaled to a common range (i.e. [0.0 1.0])
    mlp.enableScaling( true );
    
    //Add the mlp to the pipeline (after we do this, we don't need the mlp anymore)
    pipeline.setRegressifier( mlp );
}

//--------------------------------------------------------------
void testApp::update(){
    
    //Grab the current mouse x and y position
    VectorDouble inputVector(2);
    inputVector[0] = mouseX;
    inputVector[1] = mouseY;
    
    //If we are recording training data, then add the current sample to the training data set
    if( record ){
        trainingData.addSample(inputVector, targetVector);
    }
    
    //If the pipeline has been trained, then run the prediction
    if( pipeline.getTrained() ){
        pipeline.predict( inputVector );
    }

}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0, 0, 0);
    
    string text;
    int textX = 20;
    int textY = 20;
    
    //Draw the training info
    ofSetColor(255, 255, 255);
    text = "------------------- TrainingInfo -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    if( record ) ofSetColor(255, 0, 0);
    else ofSetColor(255, 255, 255);
    textY += 15;
    text = record ? "RECORDING" : "Not Recording";
    ofDrawBitmapString(text, textX,textY);
    
    ofSetColor(255, 255, 255);
    textY += 15;
    text = "TargetVector: ";
    for(UINT i=0; i<targetVector.size(); i++){
        text += ofToString(targetVector[i]) + "  ";
    }
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "NumTrainingSamples: " + ofToString(trainingData.getNumSamples());
    ofDrawBitmapString(text, textX,textY);
    
    
    //Draw the prediction info
    textY += 30;
    text = "------------------- Prediction Info -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text =  pipeline.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "PredictedOutputVector: ";
    VectorDouble regressionData = pipeline.getRegressionData();
    for(UINT i=0; i<regressionData.size(); i++){
        text += ofToString(regressionData[i]) + "  ";
    }
    ofDrawBitmapString(text, textX,textY);

    
    //Draw the info text
    textY += 30;
    text = "InfoText: " + infoText;
    ofDrawBitmapString(text, textX,textY);
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    infoText = "";
    
    switch ( key) {
        case 'r':
            record = !record;
            break;
        case '[':
            targetVector[0] -= 0.1;
            break;
        case ']':
            targetVector[0] += 0.1;
            break;
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";
            }else infoText = "WARNING: Failed to train pipeline";
            break;
        case 's':
            if( trainingData.saveDatasetToFile("TrainingData.txt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.loadDatasetFromFile("TrainingData.txt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            break;
        default:
            break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}