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

#pragma once
#include "ofMain.h"
#include "../../GRT/GRT.h"

namespace GRT{

class TimeSeriesGraph{
public:
    TimeSeriesGraph(unsigned int numPoints=0,unsigned int numDimensions=0);
    ~TimeSeriesGraph();
    
    bool init(unsigned int numPoints,unsigned int numDimensions);
    bool reset();
    bool setData( const vector< VectorDouble > &data );
    bool setData( const MatrixDouble &data );
    bool setRanges(double newMin,double newMax,bool lockRanges = false);
    bool setRanges(VectorDouble newMin,VectorDouble newMax,bool lockRanges = false);
    void lockRanges(bool rangesLocked){ this->rangesLocked = rangesLocked; }
    bool update();
    bool update( const VectorDouble &data );
    bool draw(unsigned int x,unsigned int y,unsigned int w,unsigned int h);
    
    unsigned int numDimensions;
    unsigned int numPoints;
    VectorDouble minRanges;
    VectorDouble maxRanges;
    vector< string > channelNames;
    vector< bool > channelVisible;
    CircularBuffer< VectorDouble > dataBuffer;
    
    bool initialized;
    bool rangesComputed;
    bool rangesLocked;
    bool constrainValuesToGraph;
    bool drawInfoText;
    bool drawGrid;
    
    ofColor gridColor;
    ofColor backgroundColor;
    vector< ofColor > colors;
    ErrorLog errorLog;
    
};

}//End of namespace GRT