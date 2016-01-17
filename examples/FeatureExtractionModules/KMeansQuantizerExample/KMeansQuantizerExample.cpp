/*
 GRT KMeansQuantizer Example
 This examples demonstrates how to use the KMeansQuantizer module. 

 The KMeansQuantizer module quantizes the N-dimensional input vector to a 1-dimensional discrete
 value. This value will be between [0 K-1], where K is the number of clusters used to create the 
 quantization model. Before you use the KMeansQuantizer, you need to train a quantization model.
 To do this, you select the number of clusters you want your quantizer to have and then give it
 any training data in the following formats:
 - ClassificationData
 - TimeSeriesClassificationData
 - TimeSeriesClassificationDataStream
 - UnlabelledClassificationData
 - MatrixDouble
 
 The example loads a basic dataset and uses this to train a quantization model. After the model 
 is trained, the data is then run through the quantizer and the quantized values are printed for
 demonstration.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main(int argc, const char * argv[])
{
	//Load a basic dataset from a file
	ClassificationData data;
	
	if( !data.load("KMeansQuantizerTrainingData.txt") ){
		cout << "ERROR: Failed to load training data!\n";
		return EXIT_FAILURE;
	}
	
    //Create a new KMeansQuantizer instance
	KMeansQuantizer quantizer( 5 );
	
	//Train the quantization model
	if( !quantizer.train( data ) ){
		cout << "ERROR: Failed to train quantizer!\n";
		return EXIT_FAILURE;
	}
	
	//Save the model and settings to a file
	if( !quantizer.save("KMeansQuantizerSettings.grt") ){
		cout << "ERROR: Failed to save settings to file!\n";
		return EXIT_FAILURE;
	}
	
	//Load the model and settings from a file
	if( !quantizer.load("KMeansQuantizerSettings.grt") ){
		cout << "ERROR: Failed to load settings from file!\n";
		return EXIT_FAILURE;
	}
	
	//Run the data through the quantizer and print out the quantized value
	for(UINT i=0; i<data.getNumSamples(); i++){
		//Quantize the i'th sample
		quantizer.quantize( data[i].getSample() );
		
		//Print out the sample values and the quantized value
		cout << "Index: " << i;
		cout << " Sample: ";
		for(UINT j=0; j<data[i].getNumDimensions(); j++){
			cout << data[i][j] << "\t";
		}
		cout << "QuantizedValue: " << quantizer.getQuantizedValue() << endl;
		
	}
	
    return EXIT_SUCCESS;
}
