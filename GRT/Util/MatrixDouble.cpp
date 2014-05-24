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

#include "MatrixDouble.h"

namespace GRT{
   
MatrixDouble::MatrixDouble(){
    warningLog.setProceedingText("[WARNING MatrixDouble]");
    errorLog.setProceedingText("[ERROR MatrixDouble]");
    this->dataPtr = NULL;
    this->rows = 0;
    this->cols = 0;
}
    
MatrixDouble::MatrixDouble(const unsigned int rows,const unsigned int cols){
    warningLog.setProceedingText("[WARNING MatrixDouble]");
    errorLog.setProceedingText("[ERROR MatrixDouble]");
    this->dataPtr = NULL;
    if( rows > 0 && cols > 0 ){
        resize(rows, cols);
    }
}
    
MatrixDouble::MatrixDouble(const MatrixDouble &rhs){
    warningLog.setProceedingText("[WARNING MatrixDouble]");
    errorLog.setProceedingText("[ERROR MatrixDouble]");
    this->dataPtr = NULL;
    this->rows = rhs.rows;
    this->cols = rhs.cols;
    if( rhs.dataPtr != NULL ){
        this->dataPtr = new double*[rows];
        for(unsigned int i=0; i<rows; i++){
            dataPtr[i] = new double[cols];
            for(unsigned int j=0; j<cols; j++)
                this->dataPtr[i][j] = rhs.dataPtr[i][j];
        }
    }
    
}
    
MatrixDouble::MatrixDouble(const Matrix<double> &rhs){
    warningLog.setProceedingText("[WARNING MatrixDouble]");
    errorLog.setProceedingText("[ERROR MatrixDouble]");
    this->dataPtr = NULL;
    this->rows = rhs.getNumRows();
    this->cols = rhs.getNumCols();
    if( this->rows > 0 && this->cols > 0 ){
        this->dataPtr = new double*[rows];
        for(unsigned int i=0; i<rows; i++){
            dataPtr[i] = new double[cols];
            for(unsigned int j=0; j<cols; j++)
                this->dataPtr[i][j] = rhs[i][j];
        }
    }
}

MatrixDouble::~MatrixDouble(){
    clear();
}
    
MatrixDouble& MatrixDouble::operator=(const MatrixDouble &rhs){
    if( this != &rhs ){
        this->clear();
        this->rows = rhs.rows;
        this->cols = rhs.cols;
        dataPtr = new double*[rows];
        for(unsigned int i=0; i<rows; i++){
            dataPtr[i] = new double[cols];
            for(unsigned int j=0; j<cols; j++)
                this->dataPtr[i][j] = rhs.dataPtr[i][j];
        }
    }
    return *this;
}
    
MatrixDouble& MatrixDouble::operator=(const Matrix<double> &rhs){
    if( this != &rhs ){
        this->clear();
        this->rows = rhs.getNumRows();
        this->cols = rhs.getNumCols();
        dataPtr = new double*[rows];
        for(unsigned int i=0; i<rows; i++){
            dataPtr[i] = new double[cols];
            for(unsigned int j=0; j<cols; j++)
                this->dataPtr[i][j] = rhs[i][j];
        }
    }
    return *this;
}
    
MatrixDouble& MatrixDouble::operator=(const vector< VectorDouble> &rhs){
    
    clear();
    
    if( rhs.size() == 0 ) return *this;
    
    unsigned int M = (unsigned int)rhs.size();
    unsigned int N = (unsigned int)rhs[0].size();
    resize(M, N);
    
    for(unsigned int i=0; i<M; i++){
        if( rhs[i].size() != N ){
            clear();
            return *this;
        }
        for(unsigned int j=0; j<N; j++){
            dataPtr[i][j] = rhs[i][j];
        }
    }
    
    return *this;
}
    
bool MatrixDouble::resize(const unsigned int rows,const unsigned int cols){
    return Matrix<double>::resize(rows, cols);
}
 
bool MatrixDouble::print(const string title){
    
    if( dataPtr == NULL ) return false;
    
    if( title != "" ){
        std::cout << title << endl;
    }
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            std::cout << dataPtr[i][j] << "\t";
        }
        std::cout << std::endl;
    }

    return true;
}
    
bool MatrixDouble::transpose(){
    
    if( dataPtr == NULL ) return false;
    
    MatrixDouble temp(cols,rows);
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            temp[j][i] = dataPtr[i][j];
        }
    }
    
    *this = temp;
    
    return true;
}
    
bool MatrixDouble::scale(const double minTarget,const double maxTarget){
    
    if( dataPtr == NULL ) return false;
    
    vector< MinMax > ranges = getRanges();
    
    return scale(ranges,minTarget,maxTarget);
}
    
bool MatrixDouble::scale(const vector< MinMax > &ranges,const double minTarget,const double maxTarget){
    if( dataPtr == NULL ) return false;
    
    if( ranges.size() != cols ){
        return false;
    }
    
    for(UINT i=0; i<rows; i++){
        for(UINT j=0; j<cols; j++){
            dataPtr[i][j] = Util::scale(dataPtr[i][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
        }
    }
    return true;
}
    
bool MatrixDouble::znorm(const double alpha){
    if( dataPtr == NULL ) return false;
    
    UINT i,j = 0;
    double mean, std = 0;
    for(i=0; i<rows; i++){
        mean = 0;
        std = 0;
        
        //Compute the mean
        for(j=0; j<cols; j++){
            mean += dataPtr[i][j];
        }
        mean /= cols;
        
        //Compute the std dev
        for(j=0; j<cols; j++){
            std += (dataPtr[i][j]-mean)*(dataPtr[i][j]-mean);
        }
        std /= cols;
        std = sqrt( std + alpha );
        
        //Normalize the row
        for(j=0; j<cols; j++){
            dataPtr[i][j] = (dataPtr[i][j]-mean) / std;
        }
    }
    
    return true;
}
    
MatrixDouble MatrixDouble::multiple(const double value) const{
    
    if( dataPtr == NULL ) return MatrixDouble();
    
    MatrixDouble d(rows,cols);
    
    unsigned int i,j = 0;
    for( i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            d[i][j] = dataPtr[i][j] * value;
        }
    }
    
    return d;
}
    
VectorDouble MatrixDouble::multiple(const VectorDouble &b) const{
    
    const unsigned int M = rows;
    const unsigned int N = cols;
    const unsigned int K = (unsigned int)b.size();
    
    if( N != K ){
        warningLog << "multiple(vector b) - The size of b (" << b.size() << ") does not match the number of columns in this matrix (" << N << ")" << std::endl;
        return VectorDouble();
    }
    
    VectorDouble c(M);
    const double *pb = &b[0];
    double *pc = &c[0];
    
    unsigned int i,j = 0;
    for(i=0; i<rows; i++){
        pc[i] = 0;
        for(j=0; j<cols; j++){
            pc[i] += dataPtr[i][j]*pb[j];
        }
    }
    
    return c;
}
    
MatrixDouble MatrixDouble::multiple(const MatrixDouble &b) const{
    
    const unsigned int M = rows;
    const unsigned int N = cols;
    const unsigned int K = b.getNumRows();
    const unsigned int L = b.getNumCols();
    
    if( N != K ) {
        errorLog << "multiple(MatrixDouble b) - The number of rows in b (" << K << ") does not match the number of columns in this matrix (" << N << ")" << std::endl;
        return MatrixDouble();
    }
    
    MatrixDouble c(M,L);
    double **pb = b.getDataPointer();
    double **pc = c.getDataPointer();
    
    unsigned int i,j,k = 0;
    for(i=0; i<M; i++){
        for(j=0; j<L; j++){
            pc[i][j] = 0;
            for(k=0; k<K; k++){
                pc[i][j] += dataPtr[i][k] * pb[k][j];
            }
        }
    }
    
    return c;
}
    
bool MatrixDouble::multiple(const MatrixDouble &a,const MatrixDouble &b,const bool aTranspose){
    
    const unsigned int M = !aTranspose ? a.getNumRows() : a.getNumCols();
    const unsigned int N = !aTranspose ? a.getNumCols() : a.getNumRows();
    const unsigned int K = b.getNumRows();
    const unsigned int L = b.getNumCols();
    
    if( N != K ) {
        errorLog << "multiple(const MatrixDouble &a,const MatrixDouble &b,const bool aTranspose) - The number of rows in a (" << K << ") does not match the number of columns in matrix b (" << N << ")" << std::endl;
        return false;
    }
    
    if( !resize( M, L ) ){
        errorLog << "multiple(const MatrixDouble &b,const MatrixDouble &c,const bool bTranspose) - Failed to resize matrix!" << endl;
        return false;
    }
    
    unsigned int i, j, k = 0;
    
    //Using direct pointers really helps speed up the computation time
    double **pa = a.getDataPointer();
    double **pb = b.getDataPointer();
    
    if( aTranspose ){
        
        for(j=0; j<L; j++){
            for(i=0; i<M; i++){
                dataPtr[i][j] = 0;
                for(k=0; k<K; k++){
                    dataPtr[i][j] += pa[k][i] * pb[k][j];
                }
            }
        }
        
    }else{
        
        for(j=0; j<L; j++){
            for(i=0; i<M; i++){
                dataPtr[i][j] = 0;
                for(k=0; k<K; k++){
                    dataPtr[i][j] += pa[i][k] * pb[k][j];
                }
            }
        }
        
    }
    
    return true;
}
    
bool MatrixDouble::add(const MatrixDouble &b){
    
    if( b.getNumRows() != rows ){
        errorLog << "add(const MatrixDouble &b) - Failed to add matrix! The rows do not match!" << endl;
        return false;
    }
    
    if( b.getNumCols() != cols ){
        errorLog << "add(const MatrixDouble &b) - Failed to add matrix! The rows do not match!" << endl;
        return false;
    }
    
    unsigned int i,j;
    
    //Using direct pointers really helps speed up the computation time
    double **pb = b.getDataPointer();
    
    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            dataPtr[i][j] += pb[i][j];
        }
    }
    
    return true;
}
    
bool MatrixDouble::add(const MatrixDouble &a,const MatrixDouble &b){
    
    const unsigned int M = a.getNumRows();
    const unsigned int N = a.getNumCols();
    
    if( M != b.getNumRows() ){
        errorLog << "add(const MatrixDouble &a,const MatrixDouble &b) - Failed to add matrix! The rows do not match!";
        errorLog << " a rows: " << M << " b rows: " << b.getNumRows() << endl;
        return false;
    }
    
    if( N != b.getNumCols() ){
        errorLog << "add(const MatrixDouble &a,const MatrixDouble &b) - Failed to add matrix! The columns do not match!";
        errorLog << " a cols: " << N << " b cols: " << b.getNumCols() << endl;
        return false;
    }
    
    resize( M, N );
    
    UINT i,j;
    
    //Using direct pointers really helps speed up the computation time
    double **pa = a.getDataPointer();
    double **pb = b.getDataPointer();
    
    for(i=0; i<M; i++){
        for(j=0; j<N; j++){
            dataPtr[i][j] = pa[i][j] + pb[i][j];
        }
    }
    
    return true;
}
    
bool MatrixDouble::subtract(const MatrixDouble &b){
    
    if( b.getNumRows() != rows ){
        errorLog << "subtract(const MatrixDouble &b) - Failed to add matrix! The rows do not match!" << endl;
        errorLog << " rows: " << rows << " b rows: " << b.getNumRows() << endl;
        return false;
    }
    
    if( b.getNumCols() != cols ){
        errorLog << "subtract(const MatrixDouble &b) - Failed to add matrix! The rows do not match!" << endl;
        errorLog << "  cols: " << cols << " b cols: " << b.getNumCols() << endl;
        return false;
    }
    
    unsigned int i,j;
    
    //Using direct pointers really helps speed up the computation time
    double **pb = b.getDataPointer();
    
    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){
            dataPtr[i][j] -= pb[i][j];
        }
    }
    
    return true;
}
    
bool MatrixDouble::subtract(const MatrixDouble &a,const MatrixDouble &b){
    
    const unsigned int M = a.getNumRows();
    const unsigned int N = a.getNumCols();
    
    if( M != b.getNumRows() ){
        errorLog << "subtract(const MatrixDouble &a,const MatrixDouble &b) - Failed to add matrix! The rows do not match!";
        errorLog << " a rows: " << M << " b rows: " << b.getNumRows() << endl;
        return false;
    }
    
    if( N != b.getNumCols() ){
        errorLog << "subtract(const MatrixDouble &a,const MatrixDouble &b) - Failed to add matrix! The columns do not match!";
        errorLog << " a cols: " << N << " b cols: " << b.getNumCols() << endl;
        return false;
    }
    
    resize( M, N );
    
    UINT i,j;
    
    //Using direct pointers really helps speed up the computation time
    double **pa = a.getDataPointer();
    double **pb = b.getDataPointer();
    
    for(i=0; i<M; i++){
        for(j=0; j<N; j++){
            dataPtr[i][j] = pa[i][j] - pb[i][j];
        }
    }
    
    return true;
}
    
double MatrixDouble::getMinValue() const{
    double minValue = 99e+99;
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            if( dataPtr[i][j] < minValue ) minValue = dataPtr[i][j];
        }
    }
    return minValue;
}

double MatrixDouble::getMaxValue() const{
    double maxValue = 99e-99;
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            if( dataPtr[i][j] > maxValue ) maxValue = dataPtr[i][j];
        }
    }
    return maxValue;
}
    
VectorDouble MatrixDouble::getMean() const{
    
    VectorDouble mean(cols);
    
    for(unsigned int c=0; c<cols; c++){
        mean[c] = 0;
        for(unsigned int r=0; r<rows; r++){
            mean[c] += dataPtr[r][c];
        }
        mean[c] /= double( rows );
    }
    
    return mean;
}
    
VectorDouble MatrixDouble::getStdDev() const{
    
    VectorDouble mean = getMean();
	VectorDouble stdDev(cols,0);
	
	for(unsigned int j=0; j<cols; j++){
		for(unsigned int i=0; i<rows; i++){
			stdDev[j] += (dataPtr[i][j]-mean[j])*(dataPtr[i][j]-mean[j]);
		}
		stdDev[j] = sqrt( stdDev[j] / double(rows-1) );
	}
    return stdDev;
}

MatrixDouble MatrixDouble::getCovarianceMatrix() const{
    
    vector<double> mean = getMean();
    MatrixDouble covMatrix(cols,cols);
    
    for(unsigned int j=0; j<cols; j++){
        for(unsigned int k=0; k<cols; k++){
            covMatrix[j][k] = 0;
            for(unsigned int i=0; i<rows; i++){
                covMatrix[j][k] += (dataPtr[i][j]-mean[j]) * (dataPtr[i][k]-mean[k]);
            }
            covMatrix[j][k] /= double(rows-1);
        }
    }
    
    return covMatrix;
}
    
std::vector< MinMax > MatrixDouble::getRanges() const{
    
    if( rows == 0 ) return std::vector< MinMax >();
    
    vector< MinMax > ranges(cols);
    for(unsigned int i=0; i<rows; i++){
        for(unsigned int j=0; j<cols; j++){
            ranges[j].updateMinMax( dataPtr[i][j] );
        }
    }
    return ranges;
}
    
double MatrixDouble::getTrace() const{
    double t = 0;
    unsigned int K = (rows < cols ? rows : cols);
    for(unsigned int i=0; i < K; i++) {
        t += dataPtr[i][i];
    }
    return t;
}
    
bool MatrixDouble::saveToCSVFile(const string &filename) const{
    
    std::fstream file;
	file.open(filename.c_str(), std::ios::out);
    
	if( !file.is_open() ){
		return false;
	}
    
	for(UINT i=0; i<rows; i++){
		for(UINT j=0; j<cols; j++){
			file << dataPtr[i][j] << (j<cols-1 ? "," : "\n");
		}
	}
    
	file.close();
    return true;
}
    
bool MatrixDouble::loadFromCSVFile(const string &filename,const char seperator){
    
    //Clear any previous data
    clear();
    
    //Open the file
    ifstream file( filename.c_str(), ifstream::in );
    if ( !file.is_open() ){
        warningLog << "parseFile(...) - Failed to open file: " << filename << endl;
        return false;
    }
    
    vector< string > vec;
    vector< double > row;
    string line;
    string columnString = "";
    const int sepValue = seperator;
    unsigned int rowCounter = 0;
    unsigned int columnCounter = 0;
    unsigned int length = 0;
    
    //Count the number of columns in the first row
    if( !getline(file,line) ){
        warningLog << "parseFile(...) - Failed to read first row!" << endl;
        return false;
    }
    
    length = (unsigned int)line.length();
    for(unsigned int i=0; i<length; i++){
        if( int(line[i]) == sepValue ){
            columnCounter++;
        }
    }
    columnCounter++;
    
    //Count the number of rows in the file
    rowCounter = 1;
    while ( getline(file,line) ){
        rowCounter++;
    }
    
    //Assign the memory
    if( !resize(rowCounter, columnCounter) ){
        warningLog << "parseFile(...) - Failed to resize memory!" << endl;
        return false;
    }
    
    //Reset the file read pointer
    file.close();
    file.open( filename.c_str(), ifstream::in );
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
            warningLog << " ColumnSize: " << columnCounter << " LastColumnSize: " << vec.size() << endl;
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
        
        //Convert the string column values to double values
        for(unsigned int j=0; j<columnCounter; j++){
            dataPtr[rowCounter][j] = stringToDouble(vec[j]);
        }
        rowCounter++;
    }
    
    //Close the file
    file.close();
    
    return true;
}
    
}; //End of namespace GRT