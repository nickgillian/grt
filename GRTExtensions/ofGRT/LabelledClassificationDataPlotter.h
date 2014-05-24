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
    
class LabelledClassificationDataPlotter{
public:
    LabelledClassificationDataPlotter();
    ~LabelledClassificationDataPlotter();
    
    bool init(LabelledClassificationData &data,unsigned int viewMode = VIEW_ND_PLOT,double dataPointSize = 3);
    bool draw(unsigned int x,unsigned int y,unsigned int w,unsigned int h);
    
    double inline scale(double x,double minSource,double maxSource,double minTarget,double maxTarget){
        return (((x-minSource)*(maxTarget-minTarget))/(maxSource-minSource))+minTarget;
    }
    
    bool initialized;
    bool drawInfoText;
    unsigned int viewMode;
    double dataPointSize;
    LabelledClassificationData *dataPtr;
    ofColor backgroundColor;
    ofColor gridColor;
    vector< ofColor > classColors;
    
    enum ViewModes{VIEW_ND_PLOT=0,VIEW_2D_PLOT,VIEW_3D_PLOT};
};

}//End of namespace GRT