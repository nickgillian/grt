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

/*
 GRT Self Organizing Map Example
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create a new KMeans instance
    SelfOrganizingMap som(64,SelfOrganizingMap::RANDOM_NETWORK,200);

	//There are a number of ways of training the KMeans algorithm, depending on what you need the KMeans for
	//These are:
	//- with labelled training data (in the ClassificationData format)
	//- with unlablled training data (in the UnlabelledData format)
	//- with unlabelled training data (in a simple MatrixDouble format)
	
	//This example shows you how to train the algorithm with unlabelled data (a simple matrix of RGB color values)
	
	MatrixFloat data;
	VectorFloat row(3);
	Random random;

	for(UINT i=0; i<1000; i++){
		Float r = random.getRandomNumberUniform(0.0,1.0);
		Float g = 0;
		Float b = 1.0-r;
		row[0] = r; row[1] = g; row[2] = b; data.push_back( row );
	}

	/*

	row[0] = 0; row[1] = 0; row[2] = 0; data.push_back( row );
	row[0] = 0; row[1] = 0; row[2] = 1; data.push_back( row );
	row[0] = 0.125; row[1] = 0.529; row[2] = 1.0; data.push_back( row );
	row[0] = 0.33; row[1] = 0.4; row[2] = 0.67; data.push_back( row );
	row[0] = 0.6; row[1] = 0.5; row[2] = 1.0; data.push_back( row );
	row[0] = 0; row[1] = 1; row[2] = 0; data.push_back( row );
	row[0] = 1; row[1] = 0; row[2] = 0; data.push_back( row );
	row[0] = 0; row[1] = 1; row[2] = 1; data.push_back( row );
	row[0] = 1; row[1] = 0; row[2] = 1; data.push_back( row );
	row[0] = 1; row[1] = 1; row[2] = 0; data.push_back( row );
	row[0] = 1; row[1] = 1; row[2] = 1; data.push_back( row );
	row[0] = .33; row[1] = .33; row[2] = .33; data.push_back( row );
	row[0] = .5; row[1] = .5; row[2] = .5; data.push_back( row );
	row[0] = .66; row[1] = .66; row[2] = .66; data.push_back( row );

	*/
	
    //Train the KMeans algorithm - K will automatically be set to the number of classes in the training dataset
    som.enableScaling( true );
    som.setSigmaWeight( 0.5 );
    if( !som.train( data ) ){
        cout << "Failed to train model!\n";
        return EXIT_FAILURE;
    }

    Matrix< VectorFloat > weights = som.getWeightsMatrix();

    fstream file;
    file.open( "weights.csv", fstream::out );
    if( file.is_open() ){
    	for(UINT i=0; i<weights.getNumRows(); i++){
	    	for(UINT j=0; j<weights.getNumCols(); j++){
	    		for(UINT n=0; n<weights[i][j].getSize(); n++){
	    			file << weights[i][j][n];
	    			if( n+1 < weights[i][j].getSize() ) file << ",";
	    			else file << "\n";
	    		}
	    	}
	    }
	    file.close();
    }
	
    return EXIT_SUCCESS;
}
