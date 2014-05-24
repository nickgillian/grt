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

#define BIG_POSITIVE_VALUE 99e+99
#define BIG_NEGATIVE_VALUE -99e+99

namespace GRT{
    
class BarGraph{
public:
    BarGraph();
    ~BarGraph();
    
    bool init(UINT numDimensions);
    bool init(UINT numDimensions,vector< double > minRanges,vector< double > maxRanges,bool lockRanges = false,bool constrainValuesToGraph = true);
    bool resetAxisRanges();
    bool lockRanges(bool rangesLocked);
    bool setAxisRanges(vector<double> minRanges,vector<double> maxRanges,bool lockRanges = false);
    bool update(vector<double> data);
    bool draw(UINT x,UINT y,UINT w,UINT h);
    
    UINT numDimensions;
    vector< double > minRanges;
    vector< double > maxRanges;
    vector< double > data;
    bool initialized;
    bool rangesComputed;
    bool rangesLocked;
    bool constrainValuesToGraph;
    bool drawInfoText;
    bool drawGrid;
    vector< string > channelNames;
    ofColor backgroundColor;
    ofColor gridColor;
    ofColor barColor;
    
    WarningLog warningLog;
    
};

}//End of namespace GRT