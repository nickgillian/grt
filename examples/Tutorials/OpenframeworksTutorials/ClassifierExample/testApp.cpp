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
 Classifier Example 1
 
 This example shows you how to:
 - setup a gesture recognition pipeline
 - record your own dataset and save it to a file
 - load the dataset back from a file
 - train a MinDist classification algorithm using the training dataset
 - use the trained MinDist algorithm to predict the class of real-time data
 
 This example uses the 2-dimensional [x y] coordinates from your mouse as input, but you can easily change this to whatever sensor input
 you have access to.
 
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
 - when you start the project, you will have no training data and the classifier will not be trained so you need to do three things: 
   (1) record some training data
   (2) train your pipeline
   (3) use the pipeline to predict the class of real-time data
 - Step 1:
   - to record some training data, first make sure the value beside the TrainingClassLabel is set to the class you want to record the data for
   - to change the training class label you can use the '[' and ']' keys, [ to decrease the label and ] to increase the label
   - place your mouse in a specific region of the screen (for instance the top left corner)
   - press the 'r' key to start recording the training data
   - move your mouse around that area of the screen and record a few seconds of data
   - press the 'r' key to stop recording the training data
   - change the training class label to a new label and move your mouse to another section of the screen (for instance the bottom right corner)
   - press the 'r' key to start the recording, move your mouse around that section of the screen for a few seconds, stop the recording
   - keep repeating these steps until you have recorded all the training data you want
   - when you have finished, press the 's' key to save the training data to a file
   - if you need to load the training data at a later stage, for instance when you next restart the program, press the 'l' key
 - Step 2:
   - after you have recorded your training data, you can now train your pipeline
   - to train your pipeline, press the 't' key
   - if the pipeline trained a classification model successfully then you will see the info message: Pipeline Trained, otherwise you will see the 
     warning message WARNING: Failed to train pipeline. If the training failed, then make sure you have successfully recorded the training data 
 - Step 3:
   - after you have trained the pipeline, you can now use the pipeline to predict the class of real-time data
   - if the pipeline was trained, it will automatically start to predict the class of real-time data
   - move your mouse around the screen and you should see the predicted class label change through the various classes you trained the model to predict
   - note that you might also see the predicted class label of 0. This is the special NULL GESTURE LABEL, which is output by the classifier when the 
     likelihood of a gesture is too low. See this tutorial for more info: http://www.nickgillian.com/wiki/pmwiki.php?n=GRT.AutomaticGestureSpotting
 */


#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //Initialize the training and info variables
    infoText = "";
    trainingClassLabel = 1;
    record = false;
    
    //The input to the training data will be the [x y] from the mouse, so we set the number of dimensions to 2
    trainingData.setNumDimensions( 2 );
    
    //Initialize the classifier, lets use MinDist with two clusters per class
    MinDist minDist(2);
    
    //Turn on null rejection, this lets the classifier output the predicted class label of 0 when the likelihood of a gesture is low
    minDist.enableNullRejection( true );
    
    //The null rejection coeff sets how low the likelihood should be for a null gesture
    //A low value (i.e. 1) means the real-time data has to be very close to the training data for a gesture to be detected
    //A high value (i.e. 5) means the real-time data can be quite far from the training data for a gesture to be detected
    minDist.setNullRejectionCoeff( 3 );
    
    //Add the classifier to the pipeline (after we do this, we don't need the minDist classifier anymore)
    pipeline.setClassifier( minDist );

}

//--------------------------------------------------------------
void testApp::update(){
    
    //Grab the current mouse x and y position
    VectorDouble sample(2);
    sample[0] = mouseX;
    sample[1] = mouseY;
    
    //If we are recording training data, then add the current sample to the training data set
    if( record ){
        trainingData.addSample(trainingClassLabel, sample);
    }
    
    //If the pipeline has been trained, then run the prediction
    if( pipeline.getTrained() ){
        pipeline.predict( sample );
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
    text = "TrainingClassLabel: " + ofToString(trainingClassLabel);
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
    text = "PredictedClassLabel: " + ofToString(pipeline.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline.getMaximumLikelihood());
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
            if( trainingClassLabel > 1 )
                trainingClassLabel--;
            break;
        case ']':
            trainingClassLabel++;
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