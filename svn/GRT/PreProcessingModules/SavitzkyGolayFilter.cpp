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

#include "SavitzkyGolayFilter.h"

namespace GRT{
    
//Register the SavitzkyGolayFilter module with the PreProcessing base class
RegisterPreProcessingModule< SavitzkyGolayFilter > SavitzkyGolayFilter::registerModule("SavitzkyGolayFilter");
    
SavitzkyGolayFilter::SavitzkyGolayFilter(UINT numLeftHandPoints,UINT numRightHandPoints,UINT derivativeOrder,UINT smoothingPolynomialOrder,UINT numDimensions){
    
    classType = "SavitzkyGolayFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG SavitzkyGolayFilter]");
    errorLog.setProceedingText("[ERROR SavitzkyGolayFilter]");
    warningLog.setProceedingText("[WARNING SavitzkyGolayFilter]");
    init(numLeftHandPoints,numRightHandPoints,derivativeOrder,smoothingPolynomialOrder,numDimensions);
}

SavitzkyGolayFilter::SavitzkyGolayFilter(const SavitzkyGolayFilter &rhs){
    
    this->numPoints = rhs.numPoints;
    this->numLeftHandPoints = rhs.numLeftHandPoints;
    this->numRightHandPoints = rhs.numRightHandPoints;
    this->derivativeOrder = rhs.derivativeOrder;
    this->smoothingPolynomialOrder = rhs.smoothingPolynomialOrder;
    this->data = rhs.data;
    this->yy = rhs.yy;
    this->coeff = rhs.coeff;
    
    classType = "SavitzkyGolayFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG SavitzkyGolayFilter]");
    errorLog.setProceedingText("[ERROR SavitzkyGolayFilter]");
    warningLog.setProceedingText("[WARNING SavitzkyGolayFilter]");
    
    copyBaseVariables( (PreProcessing*)&rhs );
}

SavitzkyGolayFilter::~SavitzkyGolayFilter(){

}

SavitzkyGolayFilter& SavitzkyGolayFilter::operator=(const SavitzkyGolayFilter &rhs){
	if(this!=&rhs){
        this->numPoints = rhs.numPoints;
        this->numLeftHandPoints = rhs.numLeftHandPoints;
        this->numRightHandPoints = rhs.numRightHandPoints;
        this->derivativeOrder = rhs.derivativeOrder;
        this->smoothingPolynomialOrder = rhs.smoothingPolynomialOrder;
        this->data = rhs.data;
        this->yy = rhs.yy;
        this->coeff = rhs.coeff;
        copyBaseVariables( (PreProcessing*)&rhs );
	}
	return *this;
}

bool SavitzkyGolayFilter::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getPreProcessingType() == preProcessing->getPreProcessingType() ){
        
        SavitzkyGolayFilter *ptr = (SavitzkyGolayFilter*)preProcessing;
        
        //Clone the SavitzkyGolayFilter values 
        this->numPoints = ptr->numPoints;
        this->numLeftHandPoints = ptr->numLeftHandPoints;
        this->numRightHandPoints = ptr->numRightHandPoints;
        this->derivativeOrder = ptr->derivativeOrder;
        this->smoothingPolynomialOrder = ptr->smoothingPolynomialOrder;
        this->data = ptr->data;
        this->yy = ptr->yy;
        this->coeff = ptr->coeff;
        
        //Clone the base class variables
        return copyBaseVariables( preProcessing );
    }
    
    errorLog << "clone(PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << endl;
    
    return false;
}
    
bool SavitzkyGolayFilter::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    processedData = filter( inputVector );
    
    if( processedData.size() == numOutputDimensions ) return true;
    return false;

}

bool SavitzkyGolayFilter::reset(){
    if( initialized ){
        data.setAllValues(VectorDouble(numInputDimensions,0));
        yy.clear();
        yy.resize(numInputDimensions,0);
        processedData.clear();
        processedData.resize(numInputDimensions,0);
        return true;
    }
    return false;
}
    
bool SavitzkyGolayFilter::saveModelToFile(string filename) const{
    
    if( !initialized ){
        errorLog << "saveModelToFile(string filename) - The HighPassFilter has not been initialized" << endl;
        return false;
    }
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool SavitzkyGolayFilter::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_SAVITZKY_GOLAY_FILTER_FILE_V1.0" << endl;
    
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "NumPoints: " << numPoints << endl;
    file << "NumLeftHandPoints: " << numLeftHandPoints << endl;
    file << "NumRightHandPoints: " << numRightHandPoints << endl;
    file << "DerivativeOrder: " << derivativeOrder << endl;
    file << "SmoothingPolynomialOrder: " << smoothingPolynomialOrder << endl;
    
    return true;
}

bool SavitzkyGolayFilter::loadModelFromFile(string filename){
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        file.close();
        initialized = false;
        return false;
    }
    
    file.close();
    
    return true;
}

bool SavitzkyGolayFilter::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_SAVITZKY_GOLAY_FILTER_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    //Load the number of input dimensions
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumInputDimensions header!" << endl;
        return false;     
    }
    file >> numInputDimensions;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumOutputDimensions header!" << endl;
        return false;     
    }
    file >> numOutputDimensions;
    
    //Load the numPoints
    file >> word;
    if( word != "NumPoints:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumPoints header!" << endl;
        return false;     
    }
    file >> numPoints;
    
    //Load the NumLeftHandPoints
    file >> word;
    if( word != "NumLeftHandPoints:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumLeftHandPoints header!" << endl;
        return false;     
    }
    file >> numLeftHandPoints;
    
    //Load the NumRightHandPoints
    file >> word;
    if( word != "NumRightHandPoints:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read numRightHandPoints header!" << endl;
        return false;     
    }
    file >> numRightHandPoints;
    
    //Load the DerivativeOrder
    file >> word;
    if( word != "DerivativeOrder:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read DerivativeOrder header!" << endl;
        return false;     
    }
    file >> derivativeOrder;
    
    //Load the SmoothingPolynomialOrder
    file >> word;
    if( word != "SmoothingPolynomialOrder:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read SmoothingPolynomialOrder header!" << endl;
        return false;     
    }
    file >> smoothingPolynomialOrder;
        
    //Init the filter module to ensure everything is initialized correctly
    return init(numLeftHandPoints,numRightHandPoints,derivativeOrder,smoothingPolynomialOrder,numInputDimensions);  
}
    
bool SavitzkyGolayFilter::init(UINT numLeftHandPoints,UINT numRightHandPoints,UINT derivativeOrder,UINT smoothingPolynomialOrder,UINT numDimensions){
    
    initialized = false;
    
    if( numDimensions == 0 ){
        errorLog << "init(double filterFactor,double gain,UINT numDimensions) - NumDimensions must be greater than 0!" << endl;
        return false;
    }
    
    this->numPoints = numLeftHandPoints+numRightHandPoints+1;
    this->numLeftHandPoints = numLeftHandPoints;
    this->numRightHandPoints = numRightHandPoints;
    this->derivativeOrder = derivativeOrder;
    this->smoothingPolynomialOrder = smoothingPolynomialOrder;
    coeff.resize(numPoints);
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    yy.clear();
    yy.resize(numDimensions,0);
    processedData.clear();
    processedData.resize(numDimensions,0);
    data.resize(numPoints,vector<double>(numDimensions,0));
    
    if( !calCoeff() ){
        errorLog << "init(UINT NL,UINT NR,UINT LD,UINT M,UINT numDimensions) - Failed to compute filter coefficents!" << endl;
        return false;
    }
    
    initialized = true;
    
    return true;
}

double SavitzkyGolayFilter::filter(const double x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(double x) - The filter has not been initialized!" << endl;
        return 0;
    }
    
    VectorDouble y = filter(VectorDouble(1,x));
    
    if( y.size() > 0 ) return y[0];
	return 0;
}
    
VectorDouble SavitzkyGolayFilter::filter(const VectorDouble &x){
    
    if( !initialized ){
        errorLog << "filter(const VectorDouble &x) - Not Initialized!" << endl;
        return VectorDouble();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "filter(const VectorDouble &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return VectorDouble();
    }
    
    //Add the new input data to the data buffer
    data.push_back( x );
    
    //Filter the data
    for(UINT j=0; j<x.size(); j++){
        processedData[j] = 0;
        for(UINT i=0; i<numPoints; i++) 
            processedData[j] += data[i][j] * coeff[i];
    }
    
    return processedData;
}
    
bool SavitzkyGolayFilter::calCoeff(){
    
    int np = (int)numPoints;
    int nl = (int)numLeftHandPoints;
    int nr = (int)numRightHandPoints;
    int ld = (int)derivativeOrder;
    int m = (int)smoothingPolynomialOrder;
    int i,j,k,imj,ipj,kk,mm,pos;
    double fac,sum;
    VectorDouble indx(m+1);
    MatrixDouble a(m+1,m+1);
    VectorDouble b(m+1);
    VectorDouble c(np);
    
    for (ipj=0; ipj<=(m << 1); ipj++) {
        sum=(ipj ? 0.0 : 1.0);
        
        for (k=1; k<=nr; k++) sum += pow(double(k),double(ipj));
        for (k=1; k<=nl; k++) sum += pow(double(-k),double(ipj));
        
        mm = min_(ipj,2*m-ipj);
        
        for (imj = -mm; imj<=mm; imj+=2) a[(ipj+imj)/2][(ipj-imj)/2] = sum;
    }
    
    LUDecomposition alud(a);
    for (j=0;j<m+1;j++) b[j]=0.0;
    b[ld]=1.0;
    if( !alud.solve_vector(b,b) ){
        return false;
    }
    
    for (kk=0; kk<np; kk++) c[kk]=0.0;
    for (k = -nl; k<=nr; k++) {
        sum=b[0];
        fac=1.0;
        
        for(mm=1; mm<=m; mm++) 
            sum += b[mm]*(fac *= k);
        
        kk=(np-k) % np;
        c[kk]=sum;
    }
    
    //Reorder coefficients and place them in coeff
    //Reorder last=0 future = np-1
    pos = nl;
    for(i=0; i<np; i++){
        coeff[i] = c[pos--];
        if(pos==0)pos=np-1;
    }
    return true;
}

}//End of namespace GRT