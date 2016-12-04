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
#include "MovementTrajectoryFeatures.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string MovementTrajectoryFeatures::id = "MovementTrajectoryFeatures";
std::string MovementTrajectoryFeatures::getId() { return MovementTrajectoryFeatures::id; }

//Register the ZeroCrossingCounter module with the FeatureExtraction base class
RegisterFeatureExtractionModule< MovementTrajectoryFeatures > MovementTrajectoryFeatures::registerModule( MovementTrajectoryFeatures::getId() );

MovementTrajectoryFeatures::MovementTrajectoryFeatures(const UINT trajectoryLength,const UINT numCentroids,const UINT featureMode,const UINT numHistogramBins,const UINT numDimensions,const bool useTrajStartAndEndValues,const bool useWeightedMagnitudeValues) : FeatureExtraction( MovementTrajectoryFeatures::getId() )
{
    init(trajectoryLength,numCentroids,featureMode,numHistogramBins,numDimensions,useTrajStartAndEndValues,useWeightedMagnitudeValues);
}

MovementTrajectoryFeatures::MovementTrajectoryFeatures(const MovementTrajectoryFeatures &rhs) : FeatureExtraction( MovementTrajectoryFeatures::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

MovementTrajectoryFeatures::~MovementTrajectoryFeatures(){
    
}

MovementTrajectoryFeatures& MovementTrajectoryFeatures::operator=(const MovementTrajectoryFeatures &rhs){
    if(this!=&rhs){
        this->trajectoryLength = rhs.trajectoryLength;
        this->numCentroids = rhs.numCentroids;
        this->featureMode = rhs.featureMode;
        this->numHistogramBins = rhs.numHistogramBins;
        this->useTrajStartAndEndValues = rhs.useTrajStartAndEndValues;
        this->useWeightedMagnitudeValues = rhs.useWeightedMagnitudeValues;
        this->trajectoryDataBuffer = rhs.trajectoryDataBuffer;
        this->centroids = rhs.centroids;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool MovementTrajectoryFeatures::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *dynamic_cast<const MovementTrajectoryFeatures*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool MovementTrajectoryFeatures::computeFeatures(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    featureVector = update( inputVector );
    
    return true;
}

bool MovementTrajectoryFeatures::reset(){
    if( initialized ){
        return init(trajectoryLength,numCentroids,featureMode,numHistogramBins,numInputDimensions,useTrajStartAndEndValues,useWeightedMagnitudeValues);
    }
    return false;
}

bool MovementTrajectoryFeatures::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_MOVEMENT_TRAJECTORY_FEATURES_FILE_V1.0" << std::endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Write the movement trajectory settings to the file
    file << "TrajectoryLength: " << trajectoryLength << std::endl;
    file << "NumCentroids: " << numCentroids << std::endl;
    file << "FeatureMode: " << featureMode << std::endl;
    file << "NumHistogramBins: " << numHistogramBins << std::endl;
    file << "UseTrajStartAndEndValues: " << useTrajStartAndEndValues << std::endl;
    file << "UseWeightedMagnitudeValues: " << useWeightedMagnitudeValues << std::endl;
    
    return true;
}

bool MovementTrajectoryFeatures::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_MOVEMENT_TRAJECTORY_FEATURES_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    //Load the TrajectoryLength
    file >> word;
    if( word != "TrajectoryLength:" ){
        errorLog << "load(fstream &file) - Failed to read TrajectoryLength header!" << std::endl;
        return false;
    }
    file >> trajectoryLength;
    
    //Load the NumCentroids
    file >> word;
    if( word != "NumCentroids:" ){
        errorLog << "load(fstream &file) - Failed to read NumCentroids header!" << std::endl;
        return false;
    }
    file >> numCentroids;
    
    //Load the FeatureMode
    file >> word;
    if( word != "FeatureMode:" ){
        errorLog << "load(fstream &file) - Failed to read FeatureMode header!" << std::endl;
        return false;
    }
    file >> featureMode;
    
    //Load the NumHistogramBins
    file >> word;
    if( word != "NumHistogramBins:" ){
        errorLog << "load(fstream &file) - Failed to read NumHistogramBins header!" << std::endl;
        return false;
    }
    file >> numHistogramBins;
    
    //Load the UseTrajStartAndEndValues
    file >> word;
    if( word != "UseTrajStartAndEndValues:" ){
        errorLog << "load(fstream &file) - Failed to read UseTrajStartAndEndValues header!" << std::endl;
        return false;
    }
    file >> useTrajStartAndEndValues;
    
    //Load the UseWeightedMagnitudeValues
    file >> word;
    if( word != "UseWeightedMagnitudeValues:" ){
        errorLog << "load(fstream &file) - Failed to read UseWeightedMagnitudeValues header!" << std::endl;
        return false;
    }
    file >> useWeightedMagnitudeValues;
    
    //Init the ZeroCrossingCounter module to ensure everything is initialized correctly
    return init(trajectoryLength,numCentroids,featureMode,numHistogramBins,numInputDimensions,useTrajStartAndEndValues,useWeightedMagnitudeValues);
}

bool MovementTrajectoryFeatures::init(const UINT trajectoryLength,const UINT numCentroids,const UINT featureMode,const UINT numHistogramBins,const UINT numDimensions,const bool useTrajStartAndEndValues,const bool useWeightedMagnitudeValues){
    
    initialized = false;
    
    if( numCentroids > trajectoryLength ){
        errorLog << "init(...) - The number of centroids parameter can not be larger than the trajectory length parameter!" << std::endl;
        return false;
    }
    if( trajectoryLength % numCentroids != 0 ){
        errorLog << "init(...) - The trajectory length parameter must be divisible with no remainders by the number of centroids parameter!" << std::endl;
        return false;
    }
    
    if( featureMode == CENTROID_ANGLE_2D && numDimensions % 2 != 0 ){
        errorLog << "init(...) - If the featureMode is CENTROID_ANGLE_2D then the numberOfDimensions should be divisble by 2 (as each pair of points should represent {x,y})!" << std::endl;
        return false;
    }
    
    if( numHistogramBins == 0 && featureMode == CENTROID_ANGLE_2D ){
        errorLog << "init(...) - If the featureMode is CENTROID_ANGLE_2D then the numHistogramBins parameter must greater than 0!" << std::endl;
        return false;
    }
    
    //Setup the search variables
    this->trajectoryLength = trajectoryLength;
    this->numCentroids = numCentroids;
    this->featureMode = featureMode;
    this->numHistogramBins = numHistogramBins;
    this->numInputDimensions = numDimensions;
    this->useTrajStartAndEndValues = useTrajStartAndEndValues;
    this->useWeightedMagnitudeValues = useWeightedMagnitudeValues;
    featureDataReady = false;
    
    //Set the number of output dimensions
    numOutputDimensions = 0;
    switch( featureMode ){
        case CENTROID_VALUE:
        //In the centroid value mode the useTrajStartAndEndValues is ignored (as the start and end centroids are used by default)
        numOutputDimensions = numInputDimensions*numCentroids;
        break;
        case NORMALIZED_CENTROID_VALUE:
        numOutputDimensions = numInputDimensions*numCentroids;
        if( useTrajStartAndEndValues ){
            numOutputDimensions += numInputDimensions*2;
        }
        break;
        case CENTROID_DERIVATIVE:
        numOutputDimensions = numInputDimensions*(numCentroids-1);
        if( useTrajStartAndEndValues ){
            numOutputDimensions += numInputDimensions*2;
        }
        break;
        case CENTROID_ANGLE_2D:
        numOutputDimensions = numHistogramBins*(numDimensions/2);
        break;
        default:
        errorLog << "init(...)- Unknown featureMode!" << std::endl;
        return false;
        break;
    }
    
    if( numOutputDimensions == 0 ){
        errorLog << "init(...) - The numOutputDimensions is zero!" << std::endl;
        return false;
    }
    
    //Resize the feature Vector
    featureVector.resize(numOutputDimensions);
    
    //Resize the raw trajectory data buffer
    trajectoryDataBuffer.resize( trajectoryLength, VectorFloat(numInputDimensions,0) );
    
    //Resize the centroids buffer
    centroids.resize(numCentroids,numInputDimensions);
    
    //Flag that the zero crossing counter has been initialized
    initialized = true;
    
    return true;
}

VectorFloat MovementTrajectoryFeatures::update(const Float x){
    return update(VectorFloat(1,x));
}

VectorFloat MovementTrajectoryFeatures::update(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "update(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "update(const VectorFloat &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input Vector (" << x.getSize() << ")!" << std::endl;
        return VectorFloat();
    }
    
    //Add the new data to the trajectory data buffer
    trajectoryDataBuffer.push_back( x );
    
    //Only flag that the feature data is ready if the trajectory data is full
    if( trajectoryDataBuffer.getBufferFilled() ){
        featureDataReady = true;
    }else featureDataReady = false;
    
    //Compute the centroids
    centroids.setAllValues(0);
    
    UINT dataBufferIndex = 0;
    UINT numValuesPerCentroid = (UINT)floor(Float(trajectoryLength/numCentroids));
    for(UINT n=0; n<numInputDimensions; n++){
        dataBufferIndex = 0;
        for(UINT i=0; i<numCentroids; i++){
            for(UINT j=0; j<numValuesPerCentroid; j++){
                centroids[i][n] += trajectoryDataBuffer[dataBufferIndex++][n];
            }
            centroids[i][n] /= Float(numValuesPerCentroid);
        }
    }
    
    //Copmute the features
    UINT featureIndex = 0;
    Vector< MinMax > centroidNormValues(numInputDimensions);
    VectorFloat histSumValues;
    Vector< Vector< AngleMagnitude > >  angleMagnitudeValues;
    switch( featureMode ){
        case CENTROID_VALUE:
        //Simply set the feature Vector as the list of centroids
        for(UINT n=0; n<numInputDimensions; n++){
            for(UINT i=0; i<numCentroids; i++){
                featureVector[ featureIndex++ ] = centroids[i][n];
            }
        }
        break;
        case NORMALIZED_CENTROID_VALUE:
        for(UINT n=0; n<numInputDimensions; n++){
            
            //Find the min and max values
            for(UINT i=0; i<numCentroids; i++){
                centroidNormValues[n].updateMinMax( centroids[i][n] );
            }
            
            //Use the normalized centroids as the features
            for(UINT i=0; i<numCentroids; i++){
                if( centroidNormValues[n].maxValue != centroidNormValues[n].minValue ){
                    featureVector[ featureIndex++ ] = Util::scale(centroids[i][n],centroidNormValues[n].minValue,centroidNormValues[n].maxValue,0,1);
                }else featureVector[ featureIndex++ ] = 0;
            }
            
            //Add the start and end centroid values if needed
            if( useTrajStartAndEndValues ){
                featureVector[ featureIndex++ ] = centroids[0][n];
                featureVector[ featureIndex++ ] = centroids[numCentroids-1][n];
            }
        }
        break;
        case CENTROID_DERIVATIVE:
        for(UINT n=0; n<numInputDimensions; n++){
            
            //Compute the derivative between centroid i and centroid i+1
            for(UINT i=0; i<numCentroids-1; i++){
                featureVector[ featureIndex++ ] = centroids[i+1][n]-centroids[i][n];
            }
            
            //Add the start and end centroid values if needed
            if( useTrajStartAndEndValues ){
                featureVector[ featureIndex++ ] = centroids[0][n];
                featureVector[ featureIndex++ ] = centroids[numCentroids-1][n];
            }
        }
        break;
        case CENTROID_ANGLE_2D:
        histSumValues.resize( numInputDimensions/2, 0);
        angleMagnitudeValues.resize( numInputDimensions/2 );
        
        //Zero the feature Vector
        fill(featureVector.begin(),featureVector.end(),0);
        
        //Compute the angle and magnitude betweem each of the centroids, do this for each pair of points
        for(UINT n=0; n<numInputDimensions/2; n++){
            //Resize the nth buffer to hold the values for each centroid
            angleMagnitudeValues[n].resize(numCentroids-1);
            for(UINT i=0; i<numCentroids-1; i++){
                Util::cartToPolar(centroids[i+1][n*2]-centroids[i][n*2], centroids[i+1][n*2+1]-centroids[i][n*2+1], angleMagnitudeValues[n][i].magnitude, angleMagnitudeValues[n][i].angle);
            }
            
            //Add the angles to the histogram
            for(UINT i=0; i<numCentroids-1; i++){
                UINT histBin = 0;
                Float degreesPerBin = 360.0/numHistogramBins;
                Float binStartValue = 0;
                Float binEndValue = degreesPerBin;
                
                if( angleMagnitudeValues[n][i].angle < 0 || angleMagnitudeValues[n][i].angle  > 360.0 ){
                    warningLog << "The angle of a point is not between [0 360]. Angle: " << angleMagnitudeValues[n][i].angle << std::endl;
                    return VectorFloat();
                }
                
                //Find which hist bin the current angle is in
                while( true ){
                    if( angleMagnitudeValues[n][i].angle >= binStartValue && angleMagnitudeValues[n][i].angle  < binEndValue ){
                        break;
                    }
                    histBin++;
                    binStartValue += degreesPerBin;
                    binEndValue += degreesPerBin;
                }
                
                histSumValues[ n ] += useWeightedMagnitudeValues ? angleMagnitudeValues[n][i].magnitude : 1;
                featureVector[ n*numHistogramBins + histBin ] +=  useWeightedMagnitudeValues ? angleMagnitudeValues[n][i].magnitude : 1;
            }
            
            //Normalize the hist bins
            for(UINT n=0; n<numInputDimensions/2; n++){
                if( histSumValues[ n ] > 0 ){
                    for(UINT i=0; i<numHistogramBins; i++){
                        featureVector[ n*numHistogramBins + i  ] /=  histSumValues[ n ];
                    }
                }
            }
        }
        break;
        default:
        errorLog << "update(VectorFloat x)- Unknown featureMode!" << std::endl;
        return featureVector;
        break;
    }
    
    return featureVector;
}

CircularBuffer< VectorFloat > MovementTrajectoryFeatures::getTrajectoryData() const{
    if( initialized ){
        return trajectoryDataBuffer;
    }
    return CircularBuffer< VectorFloat >();
}

MatrixFloat MovementTrajectoryFeatures::getCentroids() const{
    if( initialized ){
        return centroids;
    }
    return MatrixFloat();
}

UINT MovementTrajectoryFeatures::getFeatureMode() const{
    if( initialized ){
        return featureMode;
    }
    return 0;
}

GRT_END_NAMESPACE
