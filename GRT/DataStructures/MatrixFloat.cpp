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

#include "MatrixFloat.h"

GRT_BEGIN_NAMESPACE
   
MatrixFloat::MatrixFloat(){
    warningLog.setProceedingText("[WARNING MatrixFloat]");
    errorLog.setProceedingText("[ERROR MatrixFloat]");
    this->dataPtr = NULL;
    this->rows = 0;
    this->cols = 0;
}
    
MatrixFloat::MatrixFloat(const unsigned int rows,const unsigned int cols){
    warningLog.setProceedingText("[WARNING MatrixFloat]");
    errorLog.setProceedingText("[ERROR MatrixFloat]");
    this->dataPtr = NULL;
    if( rows > 0 && cols > 0 ){
        resize(rows, cols);
    }
}
    
MatrixFloat::MatrixFloat(const MatrixFloat &rhs){
    warningLog.setProceedingText("[WARNING MatrixFloat]");
    errorLog.setProceedingText("[ERROR MatrixFloat]");
    this->dataPtr = NULL;
    this->rowPtr = NULL;
    this->copy( rhs );
}
    
MatrixFloat::MatrixFloat(const Matrix< float_t > &rhs){
    warningLog.setProceedingText("[WARNING MatrixFloat]");
    errorLog.setProceedingText("[ERROR MatrixFloat]");
    this->dataPtr = NULL;
    this->rowPtr = NULL;
    this->copy( rhs );
}

MatrixFloat::~MatrixFloat(){
    clear();
}
    
MatrixFloat& MatrixFloat::operator=(const MatrixFloat &rhs){
    if( this != &rhs ){
        this->clear();
        this->copy( rhs );
    }
    return *this;
}
    
MatrixFloat& MatrixFloat::operator=(const Matrix< float_t > &rhs){
    if( this != &rhs ){
        this->clear();
        this->copy( rhs );
    }
    return *this;
}
    
MatrixFloat& MatrixFloat::operator=(const Vector< VectorFloat > &rhs){
    
    clear();
    
    if( rhs.size() == 0 ) return *this;
    
    unsigned int M = rhs.getSize();
    unsigned int N = (unsigned int)rhs[0].getSize();
    resize(M, N);
    
    for(unsigned int i=0; i<M; i++){
        if( rhs[i].size() != N ){
            clear();
            return *this;
        }
        for(unsigned int j=0; j<N; j++){
            dataPtr[i*cols+j] = rhs[i][j];
        }
    }
    
    return *this;
}
  
bool MatrixFloat::print(const std::string title) const {
    
    if( dataPtr == NULL ) return false;
    
    if( title != "" ){
        std::cout << title << std::endl;
    }
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            std::cout << dataPtr[i*cols+j] << "\t";
        }
        std::cout << std::endl;
    }

    return true;
}
    
bool MatrixFloat::transpose(){
    
    if( dataPtr == NULL ) return false;
    
    MatrixFloat temp(cols,rows);
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            temp[j][i] = dataPtr[i*cols+j];
        }
    }
    
    *this = temp;
    
    return true;
}
    
bool MatrixFloat::scale(const float_t minTarget,const float_t maxTarget){
    
    if( dataPtr == NULL ) return false;
    
    Vector< MinMax > ranges = getRanges();
    
    return scale(ranges,minTarget,maxTarget);
}
    
bool MatrixFloat::scale(const Vector< MinMax > &ranges,const float_t minTarget,const float_t maxTarget){
    if( dataPtr == NULL ) return false;
    
    if( ranges.size() != cols ){
        return false;
    }
    
    unsigned int i,j = 0;
    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            dataPtr[i*cols+j] = grt_scale(dataPtr[i*cols+j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
        }
    }
    return true;
}
    
bool MatrixFloat::znorm(const float_t alpha){
    if( dataPtr == NULL ) return false;
    
    UINT i,j = 0;
    float_t mean, std = 0;
    for(i=0; i<rows; i++){
        mean = 0;
        std = 0;
        
        //Compute the mean
        for(j=0; j<cols; j++){
            mean += dataPtr[i*cols+j];
        }
        mean /= cols;
        
        //Compute the std dev
        for(j=0; j<cols; j++){
            std += (dataPtr[i*cols+j]-mean)*(dataPtr[i*cols+j]-mean);
        }
        std /= cols;
        std = sqrt( std + alpha );
        
        //Normalize the row
        for(j=0; j<cols; j++){
            dataPtr[i*cols+j] = (dataPtr[i*cols+j]-mean) / std;
        }
    }
    
    return true;
}
    
MatrixFloat MatrixFloat::multiple(const float_t value) const{
    
    if( dataPtr == NULL ) return MatrixFloat();
    
    MatrixFloat d(rows,cols);
    float_t *d_p = &(d[0][0]);
    
    unsigned int i = 0;
    for(i=0; i<rows*cols; i++){
        d_p[i] = dataPtr[i] * value;
    }
    
    return d;
}
    
VectorFloat MatrixFloat::multiple(const VectorFloat &b) const{
    
    const unsigned int M = rows;
    const unsigned int N = cols;
    const unsigned int K = (unsigned int)b.size();
    
    if( N != K ){
        warningLog << "multiple(vector b) - The size of b (" << b.size() << ") does not match the number of columns in this matrix (" << N << ")" << std::endl;
        return VectorFloat();
    }
    
    VectorFloat c(M);
    const float_t *pb = &b[0];
    float_t *pc = &c[0];
    
    unsigned int i,j = 0;
    for(i=0; i<rows; i++){
        pc[i] = 0;
        for(j=0; j<cols; j++){
            pc[i] += dataPtr[i*cols+j]*pb[j];
        }
    }
    
    return c;
}
    
MatrixFloat MatrixFloat::multiple(const MatrixFloat &b) const{
    
    const unsigned int M = rows;
    const unsigned int N = cols;
    const unsigned int K = b.getNumRows();
    const unsigned int L = b.getNumCols();
    
    if( N != K ) {
        errorLog << "multiple(MatrixFloat b) - The number of rows in b (" << K << ") does not match the number of columns in this matrix (" << N << ")" << std::endl;
        return MatrixFloat();
    }
    
    MatrixFloat c(M,L);
    float_t **pb = b.getDataPointer();
    float_t **pc = c.getDataPointer();
    
    unsigned int i,j,k = 0;
    for(i=0; i<M; i++){
        for(j=0; j<L; j++){
            pc[i][j] = 0;
            for(k=0; k<K; k++){
                pc[i][j] += dataPtr[i*cols+k] * pb[k][j];
            }
        }
    }
    
    return c;
}
    
bool MatrixFloat::multiple(const MatrixFloat &a,const MatrixFloat &b,const bool aTranspose){
    
    const unsigned int M = !aTranspose ? a.getNumRows() : a.getNumCols();
    const unsigned int N = !aTranspose ? a.getNumCols() : a.getNumRows();
    const unsigned int K = b.getNumRows();
    const unsigned int L = b.getNumCols();
    
    if( N != K ) {
        errorLog << "multiple(const MatrixFloat &a,const MatrixFloat &b,const bool aTranspose) - The number of rows in a (" << K << ") does not match the number of columns in matrix b (" << N << ")" << std::endl;
        return false;
    }
    
    if( !resize( M, L ) ){
        errorLog << "multiple(const MatrixFloat &b,const MatrixFloat &c,const bool bTranspose) - Failed to resize matrix!" << std::endl;
        return false;
    }
    
    unsigned int i, j, k = 0;
    
    //Using direct pointers really helps speed up the computation time
    float_t **pa = a.getDataPointer();
    float_t **pb = b.getDataPointer();
    
    if( aTranspose ){
        
        for(j=0; j<L; j++){
            for(i=0; i<M; i++){
                dataPtr[i*cols+j] = 0;
                for(k=0; k<K; k++){
                    dataPtr[i*cols+j] += pa[k][i] * pb[k][j];
                }
            }
        }
        
    }else{
        
        for(j=0; j<L; j++){
            for(i=0; i<M; i++){
                dataPtr[i*cols+j] = 0;
                for(k=0; k<K; k++){
                    dataPtr[i*cols+j] += pa[i][k] * pb[k][j];
                }
            }
        }
        
    }
    
    return true;
}
    
bool MatrixFloat::add(const MatrixFloat &b){
    
    if( b.getNumRows() != rows ){
        errorLog << "add(const MatrixFloat &b) - Failed to add matrix! The rows do not match!" << std::endl;
        return false;
    }
    
    if( b.getNumCols() != cols ){
        errorLog << "add(const MatrixFloat &b) - Failed to add matrix! The rows do not match!" << std::endl;
        return false;
    }
    
    unsigned int i = 0;
    
    //Using direct pointers really helps speed up the computation time
    const float_t *p_b = &(b[0][0]);
    
    for(i=0; i<rows*cols; i++){
        dataPtr[i] += p_b[i];
    }
    
    return true;
}
    
bool MatrixFloat::add(const MatrixFloat &a,const MatrixFloat &b){
    
    const unsigned int M = a.getNumRows();
    const unsigned int N = a.getNumCols();
    
    if( M != b.getNumRows() ){
        errorLog << "add(const MatrixFloat &a,const MatrixFloat &b) - Failed to add matrix! The rows do not match!";
        errorLog << " a rows: " << M << " b rows: " << b.getNumRows() << std::endl;
        return false;
    }
    
    if( N != b.getNumCols() ){
        errorLog << "add(const MatrixFloat &a,const MatrixFloat &b) - Failed to add matrix! The columns do not match!";
        errorLog << " a cols: " << N << " b cols: " << b.getNumCols() << std::endl;
        return false;
    }
    
    resize( M, N );
    
    UINT i,j;
    
    //Using direct pointers really helps speed up the computation time
    float_t *pa = a.getData();
    float_t *pb = b.getData();
    
    const unsigned int size = M*N;
    for(i=0; i<size; i++){
        dataPtr[i] = pa[i] + pb[i];
    }
    
    return true;
}
    
bool MatrixFloat::subtract(const MatrixFloat &b){
    
    if( b.getNumRows() != rows ){
        errorLog << "subtract(const MatrixFloat &b) - Failed to add matrix! The rows do not match!" << std::endl;
        errorLog << " rows: " << rows << " b rows: " << b.getNumRows() << std::endl;
        return false;
    }
    
    if( b.getNumCols() != cols ){
        errorLog << "subtract(const MatrixFloat &b) - Failed to add matrix! The rows do not match!" << std::endl;
        errorLog << "  cols: " << cols << " b cols: " << b.getNumCols() << std::endl;
        return false;
    }
    
    unsigned int i,j;
    
    //Using direct pointers really helps speed up the computation time
    float_t *pb = b.getData();
    
    const unsigned int size = rows*cols;
    for(i=0; i<size; i++){
        dataPtr[i] -= pb[i];
    }
    
    return true;
}
    
bool MatrixFloat::subtract(const MatrixFloat &a,const MatrixFloat &b){
    
    const unsigned int M = a.getNumRows();
    const unsigned int N = a.getNumCols();
    
    if( M != b.getNumRows() ){
        errorLog << "subtract(const MatrixFloat &a,const MatrixFloat &b) - Failed to add matrix! The rows do not match!";
        errorLog << " a rows: " << M << " b rows: " << b.getNumRows() << std::endl;
        return false;
    }
    
    if( N != b.getNumCols() ){
        errorLog << "subtract(const MatrixFloat &a,const MatrixFloat &b) - Failed to add matrix! The columns do not match!";
        errorLog << " a cols: " << N << " b cols: " << b.getNumCols() << std::endl;
        return false;
    }
    
    resize( M, N );
    
    UINT i,j;
    
    //Using direct pointers really helps speed up the computation time
    float_t **pa = a.getDataPointer();
    float_t **pb = b.getDataPointer();
    
    for(i=0; i<M; i++){
        for(j=0; j<N; j++){
            dataPtr[i*cols+j] = pa[i][j] - pb[i][j];
        }
    }
    
    return true;
}
    
float_t MatrixFloat::getMinValue() const{
    float_t minValue = 99e+99;
    for(unsigned int i=0; i<rows*cols; i++){
        if( dataPtr[i] < minValue ) minValue = dataPtr[i];
    }
    return minValue;
}

float_t MatrixFloat::getMaxValue() const{
    float_t maxValue = 99e-99;
    for(unsigned int i=0; i<rows*cols; i++){
        if( dataPtr[i] > maxValue ) maxValue = dataPtr[i];
    }
    return maxValue;
}
    
VectorFloat MatrixFloat::getMean() const{
    
    VectorFloat mean(cols);
    
    for(unsigned int c=0; c<cols; c++){
        mean[c] = 0;
        for(unsigned int r=0; r<rows; r++){
            mean[c] += dataPtr[r*cols+c];
        }
        mean[c] /= float_t( rows );
    }
    
    return mean;
}
    
VectorFloat MatrixFloat::getStdDev() const{
    
    VectorFloat mean = getMean();
	VectorFloat stdDev(cols,0);
	
	for(unsigned int j=0; j<cols; j++){
		for(unsigned int i=0; i<rows; i++){
			stdDev[j] += (dataPtr[i*cols+j]-mean[j])*(dataPtr[i*cols+j]-mean[j]);
		}
		stdDev[j] = sqrt( stdDev[j] / float_t(rows-1) );
	}
    return stdDev;
}

MatrixFloat MatrixFloat::getCovarianceMatrix() const{
    
    Vector<float_t> mean = getMean();
    MatrixFloat covMatrix(cols,cols);
    
    for(unsigned int j=0; j<cols; j++){
        for(unsigned int k=0; k<cols; k++){
            covMatrix[j][k] = 0;
            for(unsigned int i=0; i<rows; i++){
                covMatrix[j][k] += (dataPtr[i*cols+j]-mean[j]) * (dataPtr[i*cols+k]-mean[k]);
            }
            covMatrix[j][k] /= float_t(rows-1);
        }
    }
    
    return covMatrix;
}
    
Vector< MinMax > MatrixFloat::getRanges() const{
    
    if( rows == 0 ) return Vector< MinMax >();
    
    Vector< MinMax > ranges(cols);
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            ranges[j].updateMinMax( dataPtr[i*cols+j] );
        }
    }
    return ranges;
}
    
float_t MatrixFloat::getTrace() const{
    float_t t = 0;
    unsigned int K = (rows < cols ? rows : cols);
    for(unsigned int i=0; i < K; i++) {
        t += dataPtr[i*cols+i];
    }
    return t;
}
  
bool MatrixFloat::save(const std::string &filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !file.is_open() ){
        return false;
    }
    
    for(UINT i=0; i<rows; i++){
        for(UINT j=0; j<cols; j++){
            file << dataPtr[i*cols+j] << (j<cols-1 ? "," : "\n");
        }
    }
    
    file.close();
    return true;
}
    
bool MatrixFloat::load(const std::string &filename,const char seperator){
    
    //Clear any previous data
    clear();
    
    //Open the file
    std::ifstream file( filename.c_str(), std::ifstream::in );
    if ( !file.is_open() ){
        warningLog << "parseFile(...) - Failed to open file: " << filename << std::endl;
        return false;
    }
    
    Vector< std::string > vec;
    VectorFloat row;
    std::string line;
    std::string columnString = "";
    const int sepValue = seperator;
    unsigned int rowCounter = 0;
    unsigned int columnCounter = 0;
    unsigned int length = 0;
    
    //Count the number of columns in the first row
    if( !getline(file,line) ){
        warningLog << "parseFile(...) - Failed to read first row!" << std::endl;
        return false;
    }
    
    length = (unsigned int)line.length();
    for(unsigned int i=0; i<length; i++){
        if( int(line[i]) == sepValue ){
            columnCounter++;
        }
    }
    columnCounter++;

    std::cout << "counting rows..." << std::endl;
    
    //Count the number of rows in the file
    rowCounter = 1;
    while ( getline(file,line) ){
        rowCounter++;
    }

    std::cout << "matrix size: " << rowCounter << " " << columnCounter << std::endl;
    
    //Assign the memory
    if( !resize(rowCounter, columnCounter) ){
        warningLog << "parseFile(...) - Failed to resize memory!" << std::endl;
        return false;
    }
    
    //Reset the file read pointer
    file.close();
    file.open( filename.c_str(), std::ifstream::in );
    rowCounter = 0;
    
    //Loop over each line of data and parse the contents
    while ( getline(file,line) )
    {
        //Scan the line contents for the seperator token and parse the contents between each token
        vec.clear();
        columnString = "";
        length = (unsigned int)line.length();
        for(unsigned int i=0; i<length; i++){
            if( int(line[i]) == sepValue ){
                vec.push_back( columnString );
                columnString = "";
            }else columnString += line[i];
        }
        
        //Add the last column
        vec.push_back( columnString );
        
        //Check to make sure all the columns are consistent
        if( columnCounter != vec.size() ){
            clear();
            warningLog << "parseFile(...) - Found inconsistent column size in row " << rowCounter;
            warningLog << " ColumnSize: " << columnCounter << " LastColumnSize: " << vec.size() << std::endl;
            return false;
        }
        
        //Remove the new line character from the string in the last column
        if( vec.size() >= 1 ){
            size_t K = vec.size()-1;
            size_t foundA = vec[ K ].find('\n');
            size_t foundB = vec[K ].find('\r');
            if( foundA != std::string::npos || foundB != std::string::npos ){
                vec[ K ] = vec[ K ].substr(0,vec[ K ].length()-1);
            }
        }
        
        //Convert the string column values to float_t values
        for(unsigned int j=0; j<columnCounter; j++){
            dataPtr[rowCounter*cols+j] = grt_from_str< float_t >( vec[j] );
        }
        rowCounter++;
    }
    
    //Close the file
    file.close();
    
    return true;
}
    
bool MatrixFloat::saveToCSVFile(const std::string &filename) const{
    return save( filename );
}
    
bool MatrixFloat::loadFromCSVFile(const std::string &filename,const char seperator){
    return load( filename, seperator );
}
    
GRT_END_NAMESPACE
