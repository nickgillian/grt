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

#include "BarGraph.h"

namespace GRT{
    
BarGraph::BarGraph(){
    numDimensions = 0;
    drawGrid = false;
    initialized = false;
    rangesComputed = false;
    rangesLocked = false;
    drawInfoText = true;
    constrainValuesToGraph = true;
    backgroundColor[0] = 0;
	backgroundColor[1] = 0;
	backgroundColor[2] = 0;
	gridColor[0] = 100;
	gridColor[1] = 100;
	gridColor[2] = 100;
	barColor[0] = 255;
	barColor[1] = 0;
	barColor[2] = 0;
    
    warningLog.setProceedingText("WARNING BarGraph");
}

BarGraph::~BarGraph(){
}

bool BarGraph::init(UINT numDimensions){
    initialized = true;
    this->numDimensions = numDimensions;
    minRanges.resize(numDimensions,0);
    maxRanges.resize(numDimensions,0);;
    rangesLocked = false;
    rangesComputed = false;
    constrainValuesToGraph = true;

    return true;
}
    
bool BarGraph::init(unsigned int numDimensions,vector< double > minRanges,vector< double > maxRanges,bool lockRanges,bool constrainValuesToGraph){
    
    if( numDimensions != minRanges.size() || numDimensions != maxRanges.size() ){
        warningLog << "init(...) - numDimensions size (" << numDimensions << ") does not match minRanges size (" << minRanges.size() << ") or maxRanges size (" << maxRanges.size() << ")!" << endl;
        return false;
    }
    
    initialized = true;
    this->numDimensions = numDimensions;
    this->minRanges = minRanges;
    this->maxRanges = maxRanges;
    this->rangesLocked = lockRanges;
    this->constrainValuesToGraph = constrainValuesToGraph;
    
    return true;
}

bool BarGraph::resetAxisRanges(){
    if( !rangesLocked ){
        rangesComputed = false;
        return true;
    }
    return false;
}

bool BarGraph::lockRanges(bool rangesLocked){ 
    if( initialized ){ 
        this->rangesLocked = rangesLocked; 
        return true; 
    } 
    return false;
}

bool BarGraph::setAxisRanges(vector<double> minRanges,vector<double> maxRanges,bool lockRanges){
    if( initialized && minRanges.size() == numDimensions && maxRanges.size() == numDimensions ){
        this->minRanges = minRanges;
        this->maxRanges = maxRanges;
        this->rangesLocked = lockRanges;
        this->rangesComputed = true;
        return true;
    }
    
    warningLog << "setAxisRanges(...) - numDimensions size (" << numDimensions << ") does not match minRanges size (" << minRanges.size() << ") or maxRanges size (" << maxRanges.size() << ")!" << endl;
    return false;
}


bool BarGraph::update(vector< double > data){
    
    if( !initialized ){
        initialized = true;
        numDimensions = data.size();
        minRanges.resize(numDimensions,BIG_POSITIVE_VALUE);
        maxRanges.resize(numDimensions,BIG_NEGATIVE_VALUE);
        rangesComputed = false;
        rangesLocked = false;
    }else if( data.size() != numDimensions ){
        
        warningLog << "update(vector< double > data) - data size (" << data.size() << ") does not match numDimensions size (" << numDimensions << ")!" << endl;
        return false;
    }
    
    this->data = data;
    
    for(unsigned int n=0; n<numDimensions; n++){

            if( !rangesLocked ){
                
                if( rangesComputed ){
                    if( data[n] < minRanges[n] ){ minRanges[n] = data[n]; }
                    else if( data[n] > maxRanges[n] && data[n] != maxRanges[n] ){ maxRanges[n] = data[n]; }
                }else{
                    minRanges[n] = data[n];
                    maxRanges[n] = data[n]+(data[n]/100.0*1.0);
                }
            }
        
    }
    
    //Flag that the ranges have been computed
    rangesComputed = true;
       
    return true;
    
}


bool BarGraph::draw(unsigned int x,unsigned int y,unsigned int w,unsigned int h){
    
    if( !initialized ) return false;
    if( data.size() == 0 || data.size() != numDimensions ) return false;
    
    ofPushMatrix();
    ofEnableAlphaBlending();
    ofTranslate(x, y);
    
    //Draw the background
    ofFill();
    ofSetColor(backgroundColor[0],backgroundColor[1],backgroundColor[2]);
    ofRect(0,0,w,h);
    
    //Draw the grid if required
    if( drawGrid ){
        ofSetColor(gridColor[0],gridColor[1],gridColor[2]);
        unsigned int numVLines = 20;
        unsigned int numHLines = 10;
        
        //Draw the horizontal lines
        for(unsigned int i=0; i<numHLines; i++){
            float xStart = 0;
            float xEnd = w;
            float yStart = ofMap(i,0,numHLines,0,h);
            float yEnd = yStart;
            ofLine(xStart,yStart,xEnd,yEnd);
        }
        
        //Draw the vertical lines
        for(unsigned int i=0; i<numVLines; i++){
            float xStart = ofMap(i,0,numVLines,0,w);
            float xEnd = xStart+1;
            float yStart = 0;
            float yEnd = h;
            ofLine(xStart,yStart,xEnd,yEnd);
        }
    }
    
    double barWidth = floor(w/(numDimensions+1.0));
    double barSpacer = (w-(barWidth*numDimensions))/numDimensions;
    
    //Draw the axis lines
    ofSetColor(255,255,255);
    ofLine(-5,h,w+5,h); //X Axis
    ofLine(0,-5,0,h+5); //Y Axis
    
    //Draw the bars
    ofSetColor(barColor[0],barColor[1],barColor[2]);
    ofFill();
    double x1 = barSpacer/2.0;
    double x2 = 0;
    double y1 = 0;
    double y2 = 0;
    double barHeight = 0;
    for(unsigned int n=0; n<numDimensions; n++){
        if( minRanges[n] != maxRanges[n] ){
            barHeight = ofMap(data[n],minRanges[n],maxRanges[n],1,h-1,constrainValuesToGraph);
            x2 = barWidth;
            y1 = 0 + h-barHeight-1;
            y2 = barHeight; 
            ofRect(x1,y1,x2,y2);
        }
        
        x1 += barWidth + barSpacer;
    }
    
    ofDisableAlphaBlending();
    ofPopMatrix();
    
    return true;
}

}//End of namesapce GRT