/**
GyrOSC_GRT
Version: 1.0
Author: Nick Gillian

Info: This example provides a basic template for receiving and plotting real-time data from the iOS GyrOSC application
and then streaming this data to the GRT GUI.

To use this code you should download and install:
- the GRT GUI
- GyrOSC on your iOSC device (set the IP address as the IP of your machine, and the port to 9999)
- the Processing OSC library from: http://www.sojamo.de/libraries/oscP5/
*/

import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;
float[] gyro = new float[3];
float[] acc = new float[3];
float[] grav = new float[3];
float[] rot = new float[3];
float compass = 0;

int BUFFER_SIZE = 100;
Graph gryoGraph;
Graph accGraph;
Graph gravGraph;
Graph rotGraph;
Graph compassGraph;
PFont font;

//Set the pipeline mode (CLASSIFICATION_MODE or REGRESSION_MODE), the number of inputs and the number of outputs
final int pipelineMode = GRT.CLASSIFICATION_MODE;
final int numInputs = 9;
final int numOutputs = 1;

//Create a new GRT instance, this will initalize everything for us and send the setup message to the GRT GUI
GRT grt = new GRT( pipelineMode, numInputs, numOutputs, "127.0.0.1", 5000, 5001, true );
float[] data = new float[numInputs];
boolean gotNewAccData = false;
boolean gotNewGyroData = false;
boolean gotNewGravData = false;

void setup() {
  size(600,650);
  frameRate( 50 );
  
  font = loadFont( "Serif-48.vlw");
  
  //Setup the graphs
  float[] axisMin3D = {0,0,0};
  float[] axisMax3D = {0,0,0};
  float[] axisMin1D = {0};
  float[] axisMax1D = {0};
  color[] axisColors3D = {color(255,0,0),color(0,255,0),color(0,0,255)};
  color[] axisColors1D = {color(255,0,0)};

  axisMin3D[0] = axisMin3D[1] = axisMin3D[2] = -3.14;
  axisMax3D[0] = axisMax3D[1] = axisMax3D[2] = 3.14;
  gryoGraph = new Graph(3,BUFFER_SIZE,"Gyro",axisColors3D,axisMin3D,axisMax3D);
  
  axisMin3D[0] = axisMin3D[1] = axisMin3D[2] = -3.0;
  axisMax3D[0] = axisMax3D[1] = axisMax3D[2] = 3.0;
  accGraph = new Graph(3,BUFFER_SIZE,"Acc",axisColors3D,axisMin3D,axisMax3D);
  
  axisMin3D[0] = axisMin3D[1] = axisMin3D[2] = -1.0;
  axisMax3D[0] = axisMax3D[1] = axisMax3D[2] = 1.0;
  gravGraph = new Graph(3,BUFFER_SIZE,"Gravity",axisColors3D,axisMin3D,axisMax3D);
  
  axisMin3D[0] = axisMin3D[1] = axisMin3D[2] = -12.0;
  axisMax3D[0] = axisMax3D[1] = axisMax3D[2] = 12.0;
  rotGraph = new Graph(3,BUFFER_SIZE,"Rotation",axisColors3D,axisMin3D,axisMax3D);
  
  axisMin1D[0] = 0;
  axisMax1D[0] = 360;
  compassGraph = new Graph(1,BUFFER_SIZE,"Compass",axisColors1D,axisMin1D,axisMax1D);

  /* start oscP5, listening for incoming messages from GyroOSC at port 9999 */
  oscP5 = new OscP5(this,9999);
}

void draw() {
  background(255);  
  
  // Draw lines connecting all points
  float graphX = 50;
  float graphY = 20;
  float graphWidth = BUFFER_SIZE*5;
  float graphHeight = 100;
  
  gryoGraph.draw(graphX,graphY,graphWidth,graphHeight);
  graphY += graphHeight + 20;
  
  accGraph.draw(graphX,graphY,graphWidth,graphHeight);
  graphY += graphHeight + 20;
  
  gravGraph.draw(graphX,graphY,graphWidth,graphHeight);
  graphY += graphHeight + 20;
  
  rotGraph.draw(graphX,graphY,graphWidth,graphHeight);
  graphY += graphHeight + 20;
  
  compassGraph.draw(graphX,graphY,graphWidth,graphHeight);
  graphY += graphHeight + 20;
  
  //Send a data packet to the GRT
  if( gotNewAccData && gotNewGyroData && gotNewGravData ){
    
    data[0] = acc[0];
    data[1] = acc[1];
    data[2] = acc[2];
    
    data[3] = gyro[0];
    data[4] = gyro[1];
    data[5] = gyro[2];
    
    data[6] = grav[0];
    data[7] = grav[1];
    data[8] = grav[2];
    
    grt.sendData( data ); 
  }
}

void oscEvent(OscMessage theOscMessage) {
  /* check if theOscMessage has the address pattern we are looking for. */
  
  println("### received an osc message. with address pattern "+theOscMessage.addrPattern());
  
  if(theOscMessage.checkAddrPattern("/gyrosc/gyro")==true) {
     gyro[0] = theOscMessage.get(0).floatValue(); 
     gyro[1] = theOscMessage.get(1).floatValue(); 
     gyro[2] = theOscMessage.get(2).floatValue(); 
     gryoGraph.update( gyro );
     gotNewGyroData = true;
   }
  
  if(theOscMessage.checkAddrPattern("/gyrosc/accel")==true) {
    acc[0] = theOscMessage.get(0).floatValue(); 
    acc[1] = theOscMessage.get(1).floatValue(); 
    acc[2] = theOscMessage.get(2).floatValue(); 
    gotNewAccData = true;
    accGraph.update( acc );
  }
  
  if(theOscMessage.checkAddrPattern("/gyrosc/grav")==true) {
    grav[0] = theOscMessage.get(0).floatValue(); 
    grav[1] = theOscMessage.get(1).floatValue(); 
    grav[2] = theOscMessage.get(2).floatValue(); 
    gotNewGravData = true;
    gravGraph.update( grav );
  }
  
  if(theOscMessage.checkAddrPattern("/gyrosc/rrate")==true) {
    rot[0] = theOscMessage.get(0).floatValue(); 
    rot[1] = theOscMessage.get(1).floatValue(); 
    rot[2] = theOscMessage.get(2).floatValue(); 
    rotGraph.update( rot );
  }
  
  if(theOscMessage.checkAddrPattern("/gyrosc/comp")==true) {
    compass = theOscMessage.get(0).floatValue(); 
    float[] temp = { compass };
    compassGraph.update( temp );
  }
  
 
}
