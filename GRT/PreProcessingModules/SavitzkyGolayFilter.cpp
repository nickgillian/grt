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

#define GRT_DLL_EXPORTS
#include "SavitzkyGolayFilter.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string SavitzkyGolayFilter::id = "SavitzkyGolayFilter";
std::string SavitzkyGolayFilter::getId() { return SavitzkyGolayFilter::id; }

//Register the SavitzkyGolayFilter module with the PreProcessing base class
RegisterPreProcessingModule< SavitzkyGolayFilter > SavitzkyGolayFilter::registerModule( SavitzkyGolayFilter::getId() );

SavitzkyGolayFilter::SavitzkyGolayFilter(const UINT numLeftHandPoints,const UINT numRightHandPoints,const UINT derivativeOrder,const UINT smoothingPolynomialOrder,const UINT numDimensions) : PreProcessing( SavitzkyGolayFilter::getId() )
{
    init(numLeftHandPoints,numRightHandPoints,derivativeOrder,smoothingPolynomialOrder,numDimensions);
}

SavitzkyGolayFilter::SavitzkyGolayFilter(const SavitzkyGolayFilter &rhs) : PreProcessing( SavitzkyGolayFilter::getId() )
{
    
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
    
    if( this->getId() == preProcessing->getId() ){
        
        const SavitzkyGolayFilter *ptr = dynamic_cast<const SavitzkyGolayFilter*>(preProcessing);
        
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
    
    errorLog << "deepCopyFrom(PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << std::endl;
    
    return false;
}

bool SavitzkyGolayFilter::process(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    processedData = filter( inputVector );
    
    if( processedData.size() == numOutputDimensions ) return true;
    return false;
    
}

bool SavitzkyGolayFilter::reset(){
    if( initialized ){
        data.setAllValues(VectorFloat(numInputDimensions,0));
        yy.clear();
        yy.resize(numInputDimensions,0);
        processedData.clear();
        processedData.resize(numInputDimensions,0);
        return true;
    }
    return false;
}

bool SavitzkyGolayFilter::save(std::fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "save(std::fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_SAVITZKY_GOLAY_FILTER_FILE_V1.0" << std::endl;
    
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    file << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    file << "NumPoints: " << numPoints << std::endl;
    file << "NumLeftHandPoints: " << numLeftHandPoints << std::endl;
    file << "NumRightHandPoints: " << numRightHandPoints << std::endl;
    file << "DerivativeOrder: " << derivativeOrder << std::endl;
    file << "SmoothingPolynomialOrder: " << smoothingPolynomialOrder << std::endl;
    
    return true;
}

bool SavitzkyGolayFilter::load(std::fstream &file){
    
    if( !file.is_open() ){
        errorLog << "load(std::fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_SAVITZKY_GOLAY_FILTER_FILE_V1.0" ){
        errorLog << "load(std::fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Load the number of input dimensions
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "load(std::fstream &file) - Failed to read NumInputDimensions header!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "load(std::fstream &file) - Failed to read NumOutputDimensions header!" << std::endl;
        return false;
    }
    file >> numOutputDimensions;
    
    //Load the numPoints
    file >> word;
    if( word != "NumPoints:" ){
        errorLog << "load(std::fstream &file) - Failed to read NumPoints header!" << std::endl;
        return false;
    }
    file >> numPoints;
    
    //Load the NumLeftHandPoints
    file >> word;
    if( word != "NumLeftHandPoints:" ){
        errorLog << "load(std::fstream &file) - Failed to read NumLeftHandPoints header!" << std::endl;
        return false;
    }
    file >> numLeftHandPoints;
    
    //Load the NumRightHandPoints
    file >> word;
    if( word != "NumRightHandPoints:" ){
        errorLog << "load(std::fstream &file) - Failed to read numRightHandPoints header!" << std::endl;
        return false;
    }
    file >> numRightHandPoints;
    
    //Load the DerivativeOrder
    file >> word;
    if( word != "DerivativeOrder:" ){
        errorLog << "load(std::fstream &file) - Failed to read DerivativeOrder header!" << std::endl;
        return false;
    }
    file >> derivativeOrder;
    
    //Load the SmoothingPolynomialOrder
    file >> word;
    if( word != "SmoothingPolynomialOrder:" ){
        errorLog << "load(std::fstream &file) - Failed to read SmoothingPolynomialOrder header!" << std::endl;
        return false;
    }
    file >> smoothingPolynomialOrder;
    
    //Init the filter module to ensure everything is initialized correctly
    return init(numLeftHandPoints,numRightHandPoints,derivativeOrder,smoothingPolynomialOrder,numInputDimensions);
}

bool SavitzkyGolayFilter::init(UINT numLeftHandPoints,UINT numRightHandPoints,UINT derivativeOrder,UINT smoothingPolynomialOrder,UINT numDimensions){
    
    initialized = false;
    
    if( numDimensions == 0 ){
        errorLog << "init(Float filterFactor,Float gain,UINT numDimensions) - NumDimensions must be greater than 0!" << std::endl;
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
    data.resize(numPoints,VectorFloat(numDimensions,0));
    
    if( !calCoeff() ){
        errorLog << "init(UINT NL,UINT NR,UINT LD,UINT M,UINT numDimensions) - Failed to compute filter coefficents!" << std::endl;
        return false;
    }
    
    initialized = true;
    
    return true;
}

Float SavitzkyGolayFilter::filter(const Float x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(Float x) - The filter has not been initialized!" << std::endl;
        return 0;
    }
    
    VectorFloat y = filter(VectorFloat(1,x));
    
    if( y.size() > 0 ) return y[0];
    return 0;
}

VectorFloat SavitzkyGolayFilter::filter(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "filter(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "filter(const VectorFloat &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << std::endl;
        return VectorFloat();
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
    Float fac,sum;
    VectorFloat indx(m+1);
    MatrixDouble a(m+1,m+1);
    VectorFloat b(m+1);
    VectorFloat c(np);
    
    for (ipj=0; ipj<=(m << 1); ipj++) {
        sum=(ipj ? 0.0 : 1.0);
        
        for (k=1; k<=nr; k++) sum += pow(Float(k),Float(ipj));
            for (k=1; k<=nl; k++) sum += pow(Float(-k),Float(ipj));
                
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

VectorFloat SavitzkyGolayFilter::getFilteredData() const { return processedData; }
    
GRT_END_NAMESPACE
    