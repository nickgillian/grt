/**
Processing Regression Example
Version: Development version 1
Author: Nick Gillian

Info: This sketch demonstrates how to use Processing with the GRT GUI for regression. 
You can find out more information about how to use this code here: 

http://www.nickgillian.com/wiki/pmwiki.php/GRT/GUIProcessing
*/

//Import the P5 OSC library
import oscP5.*;
import netP5.*;

//Set the pipeline mode (CLASSIFICATION_MODE or REGRESSION_MODE), the number of inputs and the number of outputs
final int pipelineMode = GRT.REGRESSION_MODE;
final int numInputs = 2;
final int numOutputs = 1;

//Create a new GRT instance, this will initalize everything for us and send the setup message to the GRT GUI
GRT grt = new GRT( pipelineMode, numInputs, numOutputs, "127.0.0.1", 5000, 5001, true );

//Create some global variables to hold our data
float[] data = new float[ numInputs ];
float[] targetVector = new float[ numOutputs ];
PFont font;

void setup() {
  size(600,600);
  frameRate(30);
  
  //Load the font
  font = loadFont("SansSerif-48.vlw");
}

void draw() {
  background(0);  
  
  if( !grt.getInitialized() ){
    background(255,0,0);  
    println("WARNING: GRT Not Initalized. You need to call the setup function!");
    return;
  }
  
  //Draw the info text
  grt.drawInfoText(20,20);
  
  //Grab the mouse data and send it to the GRT backend via OSC
  data[0] = mouseX;
  data[1] = mouseY;
  grt.sendData( data );

}

void keyPressed(){
  
  switch( key ){
    case 'i':
      grt.init( pipelineMode, numInputs, numOutputs, "127.0.0.1", 5000, 5001, true );
      break;
    case '[': //Decrease the target vector value by 0.1
      for(int i=0; i<targetVector.length; i++)
        targetVector[i] -= 0.1;
      grt.sendTargetVector( targetVector );
      break;
    case ']': //Increase the target vector value by 0.1
      for(int i=0; i<targetVector.length; i++)
        targetVector[i] += 0.1;
      grt.sendTargetVector( targetVector );
      break;
    case 'r':
      if( grt.getRecordingStatus() ){
        grt.stopRecording();
      }else grt.startRecording();
      break;
    case 't':
      grt.startTraining();
      break;
    case 's':
      grt.saveTrainingDatasetToFile( "TrainingData.txt" );
      break;
    case 'l':
      grt.loadTrainingDatasetFromFile( "TrainingData.txt" );
      break;
    case 'c':
      grt.clearTrainingDataset();
    break;
    default:
      break;
  }
  
}
