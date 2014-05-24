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

#include "LabelledClassificationDataPlotter.h"

namespace GRT{
    
LabelledClassificationDataPlotter::LabelledClassificationDataPlotter(){
    initialized = false;
    dataPtr = NULL;
    backgroundColor[0] = 0;
	backgroundColor[1] = 0;
	backgroundColor[2] = 0;
	gridColor[0] = 100;
	gridColor[1] = 100;
	gridColor[2] = 100;
}

LabelledClassificationDataPlotter::~LabelledClassificationDataPlotter(){
}

bool LabelledClassificationDataPlotter::init(LabelledClassificationData &data,unsigned int viewMode,double dataPointSize){
    
    initialized = false;
    if( dataPointSize <= 0 ) return false;
    if( viewMode != VIEW_ND_PLOT && viewMode != VIEW_2D_PLOT && viewMode != VIEW_3D_PLOT  ) return false;
    
    initialized = true;
    this->viewMode = viewMode;
    this->dataPointSize = dataPointSize;
    dataPtr = &data;
    classColors.clear();
    
    classColors.resize(10);
    for(UINT i=0; i<classColors.size(); i++){
        classColors[i][0] = ofRandom(50,255);
		classColors[i][1] = ofRandom(50,255);
		classColors[i][2] = ofRandom(50,255);
    }
    
    return true;
}

bool LabelledClassificationDataPlotter::draw(unsigned int x,unsigned int y,unsigned int w,unsigned int h){
    
    if( !initialized ) return false;
    
    ofPushMatrix();
    ofEnableAlphaBlending();
    ofTranslate(x, y);
    
    //Draw the background
    ofFill();
    ofSetColor(backgroundColor[0],backgroundColor[1],backgroundColor[2]);
    ofRect(0,0,w,h);
    
    //Draw the main axis lines
    ofSetColor(255,255,255);
    ofLine(-5,h,w+5,h); //X Axis
    ofLine(0,-5,0,h+5); //Y Axis
    
    const unsigned int  numClasses = dataPtr->getNumClasses();
    const unsigned int  numDimensions = dataPtr->getNumDimensions();
    const unsigned int  numSamples = dataPtr->getNumSamples();
    vector< MinMax > ranges = dataPtr->getRanges();
    double minValue = 99e+99;
    double maxValue = -99e+99;
    for(unsigned int n=0; n<numDimensions; n++){
        if( ranges[n].minValue < minValue ) minValue = ranges[n].minValue;
        if( ranges[n].maxValue > maxValue ) maxValue = ranges[n].maxValue;
    }
    
    if( viewMode == VIEW_ND_PLOT ){
        
        double dimensionSpacer = floor(w/double(numDimensions));
        double x1 = dimensionSpacer/2.0;
        double y1 = 0;
                                    
        //Draw the class data
        for(unsigned int n=0; n<numDimensions; n++){
            
            //Draw the axis line
            ofSetColor(150, 150, 150);
            ofLine(x1,-5,x1,h+5); //Y Axis
            for(unsigned int m=0; m<numSamples; m++){
                //y1 = ofMap((*dataPtr)[m][n],minValue,maxValue,0,h);
                y1 = scale((*dataPtr)[m][n],minValue,maxValue,0,h);
				int r = classColors[ (*dataPtr)[m].getClassLabel() % classColors.size() ][0];
				int g = classColors[ (*dataPtr)[m].getClassLabel() % classColors.size() ][1];
				int b = classColors[ (*dataPtr)[m].getClassLabel() % classColors.size() ][2];
                ofSetColor( r, g, b );
                ofEllipse(x1, y1, dataPointSize, dataPointSize);
            }
            x1 += dimensionSpacer;
        }
    }
        
    ofDisableAlphaBlending();
    ofPopMatrix();
    
    return true;
}

}//End of namesapce GRT