#include "TimeSeriesGraph.h"

namespace GRT{
    
TimeSeriesGraph::TimeSeriesGraph(unsigned int numPoints,unsigned int numDimensions){
    initialized = false;
    rangesComputed = false;
    rangesLocked = false;
    constrainValuesToGraph = true;
    drawInfoText = false;
    drawGrid = false;    
    backgroundColor[0] = 0;
	backgroundColor[1] = 0;
	backgroundColor[2] = 0;
	gridColor[0] = 255;
	gridColor[1] = 255;
	gridColor[2] = 255;
    gridColor[3] = 100;
    
    errorLog.setProceedingText("[ERROR TimeSeriesGraph]");

    if( numPoints != 0 && numDimensions != 0 ){
        init(numPoints,numDimensions);
    }
}

TimeSeriesGraph::~TimeSeriesGraph(){
}

bool TimeSeriesGraph::init(unsigned int numPoints,unsigned int numDimensions){
    
    initialized = false;
    
    //Cleanup the old memory
    dataBuffer.clear();
    
    if( numPoints == 0 || numDimensions == 0 ) return false;
    
    //Setup the memory for the new buffer
    this->numPoints = numPoints;
    this->numDimensions = numDimensions;
    dataBuffer.resize(numPoints, VectorDouble(numDimensions,0));
    
    //Fill the buffer with empty values
    for(unsigned int i=0; i<numPoints; i++)
        dataBuffer.push_back(VectorDouble(numDimensions,0));
    
    rangesComputed = false;
    rangesLocked = false;
    minRanges.clear();
    maxRanges.clear();
    minRanges.resize(numDimensions,0);
    maxRanges.resize(numDimensions,0);
    channelNames.resize(numDimensions,"");
    
    colors.resize(numDimensions);
    for(unsigned int n=0; n<numDimensions; n++){
        colors[n][0] = ofRandom(50,255);
		colors[n][1] = ofRandom(50,255);
		colors[n][2] = ofRandom(50,255);
    }
  
    channelVisible.resize(numDimensions,true);
    initialized = true;
    
    return true;
    
}

bool TimeSeriesGraph::reset(){
        
    if( !initialized ) return false;
    
    
    if( !rangesLocked ){
        rangesComputed = false;
        for(unsigned int n=0; n<numDimensions; n++){
            minRanges[n] = 0;
            maxRanges[n] = 0;
        }
        //Clear the buffer
        dataBuffer.setAllValues(VectorDouble(numDimensions,0));
    }
    
    return true;
}
    
bool TimeSeriesGraph::setRanges(double newMin,double newMax,bool lockRanges){
    VectorDouble minRanges(numDimensions,newMin);
    VectorDouble maxRanges(numDimensions,newMax);
    return setRanges(minRanges,maxRanges,lockRanges);
}

bool TimeSeriesGraph::setRanges(VectorDouble minRanges,VectorDouble maxRanges,bool rangesLocked){
    if( initialized && minRanges.size() == numDimensions && minRanges.size() == maxRanges.size() ){
        this->minRanges = minRanges;
        this->maxRanges = maxRanges;
        this->rangesLocked = rangesLocked;
        rangesComputed = true;
        return true;
    }
    return false;
}
    
bool TimeSeriesGraph::setData( const vector< VectorDouble > &data ){
    
    const unsigned int M = (unsigned int)data.size();
    dataBuffer.reset();
    
    for(unsigned int i=0; i<M; i++){
        if( data[i].size() != numDimensions ){
            return false;
        }
        update( data[i] );
    }
    
    return true;
}
    
bool TimeSeriesGraph::setData( const MatrixDouble &data ){
    
    const unsigned int M = data.getNumRows();
    const unsigned int N = data.getNumCols();
    
    if( N != numDimensions ){
        errorLog << "setData( const MatrixDouble &data ) - The number of dimensions in the data does not match the number of dimensions in the graph!" << endl;
        return false;
    }
    
    dataBuffer.reset();
    
    for(unsigned int i=0; i<M; i++){
        update( data.getRowVector(i) );
    }
    
    return true;
}

bool TimeSeriesGraph::update(){
    
    //If the buffer has not been initialised then return false, otherwise update the buffer
    if( !initialized ) return false;
    
    //Repeat the previos value
    dataBuffer.push_back( dataBuffer[numPoints-1] );
    
    return true;
}

bool TimeSeriesGraph::update( const VectorDouble &data ){
    
    //If the buffer has not been initialised then return false, otherwise update the buffer
    if( !initialized || data.size() != numDimensions ) return false;
    
    //Add the new value to the buffer
    dataBuffer.push_back( data );
    
    //Check the min and max values
    if( !rangesLocked ){
        for(unsigned int n=0; n<numDimensions; n++){
     
            if( rangesComputed ){
                if( data[n] > maxRanges[n] ){ maxRanges[n] = data[n]; }
                else if( data[n] < minRanges[n] && data[n] != maxRanges[n] ){ minRanges[n] = data[n]; }
            }else{
                if( data[n] != 0 ){
                    minRanges[n] = data[n];
                    maxRanges[n] = data[n]+(data[n]/100.0*1.0);
                }else{
                    minRanges[n] = -0.0000001;
                    maxRanges[n] = 0.0000001;
                }
            }
            
        }
        
        //The min max values will now have been computed, even if this is the first run so it is safe to always set the following to true
        rangesComputed = true;
    }
    
    return true;
    
}
    
bool TimeSeriesGraph::draw(unsigned int x,unsigned int y,unsigned int w,unsigned int h){
    
    if( !initialized ) return false;
    
    ofPushMatrix();
    ofEnableAlphaBlending();
    ofTranslate(x, y);
    
    //Draw the background
    ofFill();
    ofSetColor(backgroundColor[0],backgroundColor[1],backgroundColor[2]);
    ofRect(0,0,w,h);
    
    //Draw the grid if required
    if( drawGrid ){
        ofSetColor(gridColor[0],gridColor[1],gridColor[2],gridColor[3]);
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
    
    //Draw the axis lines
    ofSetColor(255,255,255);
    ofLine(-5,h,w+5,h); //X Axis
    ofLine(0,-5,0,h+5); //Y Axis
    
    //Draw the timeseries
    unsigned int xPos = 0;
    unsigned int drawStartIndex = w < numPoints ? numPoints-w : 0;
    ofNoFill();
    for(unsigned int n=0; n<numDimensions; n++){
        xPos = 0;
        if( channelVisible[n] && rangesComputed ){
            ofBeginShape();
            for(unsigned int i=drawStartIndex; i<numPoints; i++){
                ofSetColor( colors[n][0],colors[n][1],colors[n][2] );
                double v = dataBuffer[i][n];
                if( minRanges[n] != maxRanges[n] ){
                    v = ofMap(v, minRanges[n], maxRanges[n], h, 0, constrainValuesToGraph);
                    ofVertex(xPos++,v);
                }else cout << " WARNING: TimeSeriesGraph::draw(). MIN RANGE == MAX RANGE!\n";
            }
            ofEndShape(false);
        }
    }
    
    if( drawInfoText ){
        stringstream info;
        for(unsigned int n=0; n<numDimensions; n++){
            if( channelVisible[n] ){
                ofSetColor(colors[n][0],colors[n][1],colors[n][2]);
                info.str("");
                info << "[" << n+1 << "]: " << channelNames[n] << " ";
                info << dataBuffer[numPoints-1][n] << " [" << minRanges[n] << " " << maxRanges[n] << "]" << endl;
                ofDrawBitmapString(info.str(),0,h + 25 + (25*n));
            }
        }
    }
    
    ofDisableAlphaBlending();
    ofPopMatrix();
    
    return true;
}

}//End of namespace GRT