/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief
 */

#ifndef GRT_PEAK_DETECTION_HEADER
#define GRT_PEAK_DETECTION_HEADER

#include "CircularBuffer.h"
#include "../PreProcessingModules/DoubleMovingAverageFilter.h"
#include "../PreProcessingModules/DeadZone.h"

namespace GRT{
    
struct PeakInfo{
    unsigned int peakType;
    unsigned int peakIndex;
    double peakValue;
    PeakInfo(unsigned int peakType = 0,unsigned int peakIndex = 0,double peakValue = 0){
        this->peakType = peakType;
        this->peakIndex = peakIndex;
        this->peakValue = peakValue;
    }
};
typedef struct PeakInfo PeakInfo;
    
#define DEFAULT_GLOBAL_MAXIMA_VALUE -1.0e+99
#define DEFAULT_GLOBAL_MINIMA_VALUE 1.0e+99
    
class PeakDetection{
public:
    PeakDetection(const UINT lowPassFilterSize = 5,const UINT searchWindowSize = 5);
    
	~PeakDetection();
    
    PeakDetection(const PeakDetection &rhs);
    
    PeakDetection& operator=(const PeakDetection &rhs);
    
    bool setSearchWindowSize(const UINT searchWindowSize);
	
    bool update( const double x );
    
    bool reset();
    
//protected:
    
    bool enableSearch;
    bool peakDetected;
    unsigned int inputTimeoutCounter;
    unsigned int inputTimeoutLimit;
    unsigned int maximaCounter;
    unsigned int minimaCounter;
    unsigned int lowPassFilterSize;
    unsigned int searchWindowSize;
    unsigned int searchHistorySize;
    double deadZoneThreshold;
    DoubleMovingAverageFilter lowPassFilter;
    DeadZone deadZoneFilter;
    CircularBuffer< double > filteredDataBuffer;
    CircularBuffer< double > firstDerivBuffer;
    CircularBuffer< double > secondDerivBuffer;
    CircularBuffer< unsigned int > peakTypesBuffer;
    vector< PeakInfo > peakInfo;
    PeakInfo globalMaximaPeakInfo;
    PeakInfo globalMinimaPeakInfo;
    
    enum PeakTypes{NO_PEAK_FOUND=0,LOCAL_MAXIMA_FOUND,LOCAL_MINIMA_FOUND,GLOBAL_MAXIMA_FOUND,GLOBAL_MINIMA_FOUND};
	
};

}//End of namespace GRT

#endif //GRT_PEAK_DETECTION_HEADER
