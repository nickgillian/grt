/**
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a basic grid search algorithm. 
 */

/**
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

#ifndef GRT_GRID_SEARCH_HEADER
#define GRT_GRID_SEARCH_HEADER

#include "../../CoreModules/MLBase.h"
#include "../../CoreModules/GestureRecognitionPipeline.h"

namespace GRT {

template< class T > 
class GridSearchRange {
public:
    GridSearchRange(){}
    GridSearchRange( const T _min, const T _max, const T _inc ):min(_min),max(_max),inc(_inc){ value = min; expired = false; }
    GridSearchRange( const GridSearchRange &rhs ){
        this->value = rhs.value;
        this->min = rhs.min;
        this->max = rhs.max;
        this->inc = rhs.inc;
        this->expired = rhs.expired;
    }

    T next(){
        if( expired ) return value;
        if( value + inc < max ) value += inc;
        else{ value = max; }
        return value;
    }

    bool reset(){
        value = min;
        expired = false;
        return true;
    }

    bool getExpired() const { return expired; }
    
    T get() { if( value >= max ) expired = true; return value; }

    T value;
    T min;
    T max;
    T inc;
    bool expired;
};

template < class T >
class GridSearchParam {
public:

    GridSearchParam( std::function< bool(T) > func, GridSearchRange<T> range ){
        this->func = func;
        this->range = range;
    }

    GridSearchParam( const GridSearchParam &rhs ){
        this->func = rhs.func;
        this->range = rhs.range;
    }

    bool reset(){
        return range.reset();
    }

    bool set(){
        if( !func ) return false;
        return func( range.get() );
    }

    bool update(){
        if( !func ) return false;
        return func( range.next() );
    }
    
    bool getExpired() const { return range.getExpired(); }

    T get(){ return range.get(); }

    std::function< bool(T) > func;
    GridSearchRange<T> range;
};

template< class T >
class GridSearch : public MLBase{
public:
    enum SearchType {MaxValueSearch=0,MinValueSearch};
    GridSearch() {
        classType = "GridSearch";
        infoLog.setProceedingText("[GridSearch]");
        debugLog.setProceedingText("[DEBUG GridSearch]");
        errorLog.setProceedingText("[ERROR GridSearch");
        trainingLog.setProceedingText("[TRAINING GridSearch]");
        warningLog.setProceedingText("[WARNING GridSearch]");
    }
    
    virtual ~GridSearch(){
        
    }

    bool addParameter( std::function< bool(unsigned int) > f , GridSearchRange< unsigned int > range ){
        params.push_back( GridSearchParam<unsigned int>( f, range ) );
        return true;
    }
    
    bool search( ){
        
        if( params.size() == 0 ){
            warningLog << "No parameters to search! Add some parameters!" << endl;
            return false;
        }

        switch( evalType ){
            case MaxValueSearch:
                bestResult = 0;
            break;
            case MinValueSearch:
                bestResult = numeric_limits< double >::max();
            break;
            default:
               errorLog << "recursive_search( unsigned int paramIndex ) - Unknown eval type!" << endl;
               return false;
            break; 
        }

        if( params.size() == 0 ) return false;
        unsigned int paramIndex = 0;
        return recursive_search( paramIndex );
    }

    double getBestResult() const { return bestResult; }

    T getBestModel() const { return bestModel; }

    bool setModel( const T &model ){
        this->model = model;
        return true;
    }

    bool setEvaluationFunction( std::function< double () > f, SearchType type = MaxValueSearch ){
        evalFunc = f;
        evalType = type;
        return true;
    }
 
protected:

    bool recursive_search( unsigned int paramIndex ){
        
        const unsigned int numParams = (UINT)params.size();
        if( paramIndex >= numParams ){
            errorLog << "recursive_search( unsigned int paramIndex ) - Param Index out of bounds!" << endl;
            return false;
        }

        recursive_reset( paramIndex );

        bool stopAfterNextIter = false;
        double result = 0.0;
        while( true ){

            //Make sure the parameter is set
            params[ paramIndex ].set();

            if( paramIndex+1 < numParams ) 
                recursive_search( paramIndex + 1 );
            
            if( paramIndex == numParams-1 ){ //If we are at the final parameter, run the evaluation

                //Evaluate the function using the current parameters
                result = evalFunc();
                
                switch( evalType ){
                    case MaxValueSearch:
                        if( result > bestResult ){
                            bestResult = result;
                            bestModel = model;
                        }
                    break;
                    case MinValueSearch:
                        if( result < bestResult ){
                            bestResult = result;
                            bestModel = model;
                        }
                    break;
                    default:
                        errorLog << "recursive_search( unsigned int paramIndex ) - Unknown eval type!" << endl;
                        return false;
                    break; 
                }
            }

            if( stopAfterNextIter ) break;

            //Update the parameter
            params[ paramIndex ].update();

            if( params[ paramIndex ].getExpired() ) stopAfterNextIter = true;
        }

        return true;
    }

    bool recursive_reset( unsigned int paramIndex ){
        const unsigned int numParams = (UINT)params.size();
        if( paramIndex >= numParams ){
            errorLog << "recursive_reset( unsigned int paramIndex ) - Param Index out of bounds!" << endl;
            return false;
        }

        if( paramIndex+1 < numParams ) 
            recursive_reset( paramIndex + 1 );

        return params[ paramIndex ].reset();
    }

    std::vector< GridSearchParam<unsigned int> > params;
    std::function< double () >  evalFunc; 
    SearchType evalType;
    double bestResult;
    T model;
    T bestModel;
};

}

#endif //GRT_MEAN_SHIFT_HEADER
