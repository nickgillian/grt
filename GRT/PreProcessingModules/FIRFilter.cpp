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
#include "FIRFilter.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string FIRFilter::id = "FIRFilter";
std::string FIRFilter::getId() { return FIRFilter::id; }

//Register the FIRFilter module with the PreProcessing base class
RegisterPreProcessingModule< FIRFilter > FIRFilter::registerModule( FIRFilter::getId() );

FIRFilter::FIRFilter(const FilterType filterType,const UINT numTaps,const Float sampleRate,const Float cutoffFrequency,const Float gain,const UINT numDimensions) : PreProcessing( FIRFilter::getId() )
{
    initialized = false;
    this->numInputDimensions = numDimensions;
    
    setFilterType( filterType );
    setNumTaps( numTaps );
    setSampleRate( sampleRate );
    setGain(gain);
    
    //Set the cutoff freq and design the filter
    switch( filterType ){
        case LPF:
        case HPF:
            setCutoffFrequency( cutoffFrequency );
            this->cutoffFrequencyLower = 0;
            this->cutoffFrequencyUpper = 0;
            //Build the filter
            buildFilter();
        break;
        case BPF:
            this->cutoffFrequency = 0;
            setCutoffFrequency(cutoffFrequency, cutoffFrequency);
            //Build the filter
            buildFilter();
        break;
    }
}

FIRFilter::FIRFilter(const FIRFilter &rhs) : PreProcessing( FIRFilter::getId() )
{
    *this = rhs;
}

FIRFilter::~FIRFilter(){
    
}

FIRFilter& FIRFilter::operator=(const FIRFilter &rhs){
    if(this!=&rhs){
        this->filterType = rhs.filterType;
        this->numTaps = rhs.numTaps;
        this->sampleRate = rhs.sampleRate;
        this->cutoffFrequency = rhs.cutoffFrequency;
        this->cutoffFrequencyLower = rhs.cutoffFrequencyLower;
        this->cutoffFrequencyUpper = rhs.cutoffFrequencyUpper;
        this->gain = rhs.gain;
        this->y = rhs.y;
        this->z = rhs.z;
        
        copyBaseVariables( (PreProcessing*)&rhs );
    }
    return *this;
}

bool FIRFilter::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getId() == preProcessing->getId() ){
        
        //Call the equals operator
        *this = *dynamic_cast<const FIRFilter*>(preProcessing);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << std::endl;
    
    return false;
}

bool FIRFilter::process(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    //Run the filter
    filter( inputVector );
    
    //Check to ensure the size of the filter results match the number of dimensions
    if( processedData.size() == numOutputDimensions ) return true;
    
    return false;
}

bool FIRFilter::reset(){
    
    //Reset the base class
    PreProcessing::reset();
    
    if( initialized ){
        //Set the data history buffer to zero
        for(UINT n=0; n<numInputDimensions; n++){
            for(UINT i=0; i<numTaps; i++){
                y[n][i] = 0;
            }
        }
    }
    
    return true;
}

bool FIRFilter::clear(){
    
    //Clear the base class
    PreProcessing::clear();
    
    y.clear();
    z.clear();
    
    return true;
}

bool FIRFilter::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Save the file header
    file << "GRT_FIR_FILTER_FILE_V1.0" << std::endl;
    
    //Save the preprocessing base variables
    if( !savePreProcessingSettingsToFile( file ) ){
        errorLog << "saveSettingsToFile(fstream &file) - Failed to save base settings to file!" << std::endl;
        return false;
    }
    
    //Save the filter settings
    file << "FilterType: " << filterType << std::endl;
    file << "NumTaps: " << numTaps << std::endl;
    file << "SampleRate: " << sampleRate << std::endl;
    file << "CutoffFrequency: " << cutoffFrequency << std::endl;
    file << "CutoffFrequencyLower: " << cutoffFrequencyLower << std::endl;
    file << "CutoffFrequencyUpper: " << cutoffFrequencyUpper << std::endl;
    file << "Gain: " << gain << std::endl;
    
    if( initialized ){
        
        //Store z, we do not need to store y
        file << "FilterCoeff: ";
        for(UINT i=0; i<numTaps; i++){
            file << z[i] << " ";
        }
        file << std::endl;
    }
    
    return true;
}

bool FIRFilter::load( std::fstream &file ){
    
    //Clear the filter
    clear();
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_FIR_FILTER_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        clear();
        return false;
    }
    
    if( !loadPreProcessingSettingsFromFile( file ) ){
        errorLog << "load(fstream &file) - Failed to load preprocessing base settings from file!" << std::endl;
        clear();
        return false;
    }
    
    //Load if the filter type
    file >> word;
    if( word != "FilterType:" ){
        errorLog << "load(fstream &file) - Failed to read FilterType header!" << std::endl;
        clear();
        return false;
    }
    UINT tmpFilterType;
    file >> tmpFilterType;
    filterType = static_cast<FilterType>(tmpFilterType);
    
    //Load if the number of taps
    file >> word;
    if( word != "NumTaps:" ){
        errorLog << "load(fstream &file) - Failed to read NumTaps header!" << std::endl;
        clear();
        return false;
    }
    file >> numTaps;
    
    //Load if the sample rate
    file >> word;
    if( word != "SampleRate:" ){
        errorLog << "load(fstream &file) - Failed to read SampleRate header!" << std::endl;
        clear();
        return false;
    }
    file >> sampleRate;
    
    //Load if the cutoffFrequency
    file >> word;
    if( word != "CutoffFrequency:" ){
        errorLog << "load(fstream &file) - Failed to read CutoffFrequency header!" << std::endl;
        clear();
        return false;
    }
    file >> cutoffFrequency;
    
    //Load if the CutoffFrequencyLower
    file >> word;
    if( word != "CutoffFrequencyLower:" ){
        errorLog << "load(fstream &file) - Failed to read CutoffFrequencyLower header!" << std::endl;
        clear();
        return false;
    }
    file >> cutoffFrequencyLower;
    
    //Load if the CutoffFrequencyUpper
    file >> word;
    if( word != "CutoffFrequencyUpper:" ){
        errorLog << "load(fstream &file) - Failed to read CutoffFrequencyUpper header!" << std::endl;
        clear();
        return false;
    }
    file >> cutoffFrequencyUpper;
    
    //Load if the Gain
    file >> word;
    if( word != "Gain:" ){
        errorLog << "load(fstream &file) - Failed to read Gain header!" << std::endl;
        clear();
        return false;
    }
    file >> gain;
    
    if( initialized ){
        
        //Setup the memory and then load z
        y.resize( numTaps, VectorFloat(numInputDimensions,0) );
        z.resize( numTaps );
        
        //Load z
        file >> word;
        if( word != "FilterCoeff:" ){
            errorLog << "load(fstream &file) - Failed to read FilterCoeff header!" << std::endl;
            clear();
            return false;
        }
        
        for(UINT i=0; i<numTaps; i++){
            file >> z[i];
        }
    }
    
    return true;
}

bool FIRFilter::buildFilter(){
    
    if( numInputDimensions == 0 ){
        errorLog << "buildFilter() - Failed to design filter, the number of inputs has not been set!" << std::endl;
        return false;
    }
    
    //Flag that the filter has not been built yet
    initialized = false;
    
    //Set the number of outputs to the number of inputs
    numOutputDimensions = numInputDimensions;
    
    //Reset the memory
    y.clear();
    z.clear();
    y.resize( numTaps, VectorFloat(numInputDimensions,0) );
    z.resize( numTaps, 0 );
    
    //Design the filter coeffients (z)
    Float alpha = 0;
    Float lambda = 0;
    Float phi = 0;
    const Float nyquist = sampleRate / 2.0;
    
    switch( filterType ){
        case LPF:
        //Design the low pass filter
        lambda = PI * cutoffFrequency / nyquist;
        for(UINT i=0; i<numTaps; i++){
            alpha = i - (numTaps - 1.0) / 2.0;
            if( alpha == 0.0 ) z[i] = lambda / PI;
            else z[i] = sin( alpha * lambda ) / (alpha * PI);
            }
        break;
        case HPF:
        //Design the high pass filter
        lambda = PI * cutoffFrequency / nyquist;
        for(UINT i=0; i<numTaps; i++){
            alpha = i - (numTaps - 1.0) / 2.0;
            if( alpha == 0.0 ) z[i] = 1.0 - lambda / PI;
            else z[i] = -sin( alpha * lambda ) / (alpha * PI);
            }
        break;
        case BPF:
        //Design the band pass filter
        lambda = PI * cutoffFrequencyLower / nyquist;
        phi = PI * cutoffFrequencyUpper / nyquist;
        for(UINT i=0; i<numTaps; i++){
            alpha = i - (numTaps - 1.0) / 2.0;
            if( alpha == 0.0 ) z[i] = (phi - lambda) / PI;
            else z[i] = (sin( alpha * phi ) - sin( alpha * lambda )) / (alpha * PI);
            }
        break;
        default:
        errorLog << "designFilter() - Failed to design filter. Unknown filter type!" << std::endl;
        return false;
        break;
    }
    
    //Init the preprocessing base class
    PreProcessing::init();
    
    return true;
}

Float FIRFilter::filter(const Float x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(const Float x) - The filter has not been initialized!" << std::endl;
        return 0;
    }
    
    //Run the main filter function
    VectorFloat result = filter( VectorFloat(1,x) );
    
    if( result.size() == 0 ){
        errorLog << "filter(const Float x) - Something went wrong, the size of the filtered vector is zero" << std::endl;
        return 0;
    }
    
    //Return the filtered value
    return result[0];
}

VectorFloat FIRFilter::filter(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "filter(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "filter(const VectorFloat &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << std::endl;
        return VectorFloat();
    }
    
    //Add the new sample to the buffer
    y.push_back( x );
    
    const UINT K = numTaps-1;
    
    //Run the filter for each input dimension
    for(UINT n=0; n<numInputDimensions; n++){
        processedData[n] = 0;
        for(UINT i=0; i<numTaps; i++){
            processedData[n] += y[K-i][n] * z[i];
        }
        processedData[n] *= gain;
    }
    
    return processedData;
}

FIRFilter::FilterType FIRFilter::getFilterType() const{
    return filterType;
}

UINT FIRFilter::getNumTaps() const{
    return numTaps;
}

Float FIRFilter::getSampleRate() const{
    return sampleRate;
}

Float FIRFilter::getCutoffFrequency() const{
    return cutoffFrequency;
}

Float FIRFilter::getCutoffFrequencyLower() const{
    return cutoffFrequencyLower;
}

Float FIRFilter::getCutoffFrequencyUpper() const{
    return cutoffFrequencyUpper;
}

Float FIRFilter::getGain() const{
    return gain;
}

Vector< VectorFloat > FIRFilter::getInputBuffer() const{
    if( initialized ){
        return y.getData();
    }
    return Vector< VectorFloat >();
}

VectorFloat FIRFilter::getFilterCoefficents() const{
    if( initialized ){
        return z;
    }
    return VectorFloat();
}

bool FIRFilter::setFilterType(const FilterType filterType){
    
    if( filterType == LPF || filterType == HPF || filterType == BPF ){
        this->filterType = filterType;
        initialized = false;
        return true;
    }
    
    errorLog << "setFilterType(const FilterType filterType) - Failed to set filter type, unknown filter type!" << std::endl;
    
    return false;
}

bool FIRFilter::setNumTaps(const UINT numTaps){
    
    if( numTaps > 0 ){
        this->numTaps = numTaps;
        initialized = false;
        return true;
    }
    
    errorLog << "setNumTaps(const UINT numTaps) - The number of taps must be greater than zero!" << std::endl;
    
    return false;
}

bool FIRFilter::setSampleRate(const Float sampleRate){
    
    if( sampleRate > 0 ){
        this->sampleRate = sampleRate;
        initialized = false;
        return true;
    }
    
    errorLog << "setSampleRate(const Float sampleRate) - The sample rate should be a positive number greater than zero!" << std::endl;
    
    return false;
}

bool FIRFilter::setCutoffFrequency(const Float cutoffFrequency){
    
    if( filterType == BPF ){
        warningLog << "setCutoffFrequency(const Float cutoffFrequency) - Setting the cutoff frequency has no effect if you are using a BPF. You should set the lower and upper cutoff frequencies instead!" << std::endl;
    }
    
    if( cutoffFrequency > 0 ){
        this->cutoffFrequency = cutoffFrequency;
        initialized = false;
        return true;
    }
    
    errorLog << "setCutoffFrequency(const Float cutoffFrequency) - The cutoffFrequency should be a positive number greater than zero!" << std::endl;
    
    return false;
}

bool FIRFilter::setCutoffFrequency(const Float cutoffFrequencyLower,const Float cutoffFrequencyUpper){
    
    if( filterType == LPF ){
        warningLog << "setCutoffFrequency(const Float cutoffFrequencyLower,const Float cutoffFrequencyUpper) - Setting the lower and upper cutoff frequency has no effect if you are using a LPF. You should set the cutoff frequency instead!" << std::endl;
    }
    
    if( filterType == HPF ){
        warningLog << "setCutoffFrequency(const Float cutoffFrequencyLower,const Float cutoffFrequencyUpper) - Setting the lower and upper cutoff frequency has no effect if you are using a HPF. You should set the cutoff frequency instead!" << std::endl;
    }
    
    if( cutoffFrequencyLower > 0 && cutoffFrequencyUpper > 0 ){
        this->cutoffFrequencyLower = cutoffFrequencyLower;
        this->cutoffFrequencyUpper = cutoffFrequencyUpper;
        initialized = false;
        return true;
    }
    
    errorLog << "setCutoffFrequency(const Float cutoffFrequencyLower,const Float cutoffFrequencyUpper) - The cutoffFrequency should be a positive number greater than zero!" << std::endl;
    
    return false;
}

bool FIRFilter::setGain(const Float gain){
    
    if( gain > 0 ){
        this->gain = gain;
        return true;
    }
    
    errorLog << "setGain(const Float gain) - The gain should be a positive number greater than zero!" << std::endl;
    
    return false;
}

GRT_END_NAMESPACE
