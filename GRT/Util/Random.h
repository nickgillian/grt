/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief This file contains the Random class, a useful wrapper for generating cross platform random functions. 
 This includes functions for uniform distributions (both integer and Float) and Gaussian distributions.
 */

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

#ifndef GRT_RANDOM_HEADER
#define GRT_RANDOM_HEADER

#include <assert.h>
#include <cstring>
#include "../Util/GRTVersionInfo.h"
#include "Timer.h"

GRT_BEGIN_NAMESPACE

class Random{
public:
	/**
	Default constructor. 
	Sets the random seed. If no seed is supplied then the seed will be set using the current system time.
	
	@param seed: sets the current seed, If no seed is supplied then the seed will be set using the current system time
	*/
    Random(unsigned long long seed = 0):v(4101842887655102017LL), w(1), storedval(0.0){
        if( seed == 0 ){
            Timer t;
            seed = (unsigned long long)t.getSystemTime();
        }
        setSeed( seed );
    }

    /**
	Default destructor.
	*/
    ~Random(){
    }
    
    /**
	Sets the current seed used to compute the random distrubutions.
	
	@param seed: sets the current seed, If no seed is supplied then the seed will be set using the current system time
	@return returns void
	*/
    void setSeed(unsigned long long seed = 0){
        if( seed == 0 ){
            Timer t;
            seed = (unsigned long long)t.getSystemTime();
        }
        v = 4101842887655102017LL;
        w = 1;
        storedval = 0;
        u = seed ^ v; int64();
        v = u; int64();
        w = v; int64();
    }
    
    /**
	Gets a random integer in the range [minRange maxRange-1], using a uniform distribution
	
	@param int minRange: the minimum value in the range (inclusive)
	@param int maxRange: the maximum value in the range (not inclusive)
	@return returns an integer in the range [minRange maxRange-1]
	*/
    inline int getRandomNumberInt(int minRange,int maxRange){
        return int( floor(getRandomNumberUniform(minRange,maxRange)) );
    }
    
    /**
     Gets a random integer from the Vector values. The probability of choosing a specific integer from the
     values Vector is given by the corresponding weight in the weights Vector. The size of the values
     Vector must match the size of the weights Vector. The weights do not need to sum to 1.
     
     For example, if the input values are: [1 2 3] and weights are: [0.7 0.2 0.1], then the 1 value would
     be randomly returned 70% of the time, the 2 value returned 20% of the time and the 3 value returned
     10% of the time.
     
     @param const Vector< int > &values: a Vector containing the N possible values the function can return
     @param const Vector< Float > &weights: the corresponding weights for the values Vector (must be the same size as the values Vector)
     @return returns a random integer from the values Vector, with a probability relative to the values weight
     */
    int getRandomNumberWeighted(const Vector< int > &values,const VectorFloat &weights){
        
        if( values.size() != weights.size() ) return 0;
        
        unsigned int N = (unsigned int)values.size();
        Vector< IndexedDouble > weightedValues( N );
        for(unsigned int i=0; i<N; i++){
            weightedValues[i].index = values[i];
            weightedValues[i].value = weights[i];
        }
        
        return getRandomNumberWeighted( weightedValues );
    }
    
    /**
     Gets a random integer from the input Vector. The probability of choosing a specific integer is given by the 
     corresponding weight of that value. The weights do not need to sum to 1.
     
     For example, if the input values are: [{1 0.7},{2 0.2}, {3 0.1}], then the 1 value would be randomly returned 
     70% of the time, the 2 value returned 20% of the time and the 3 value returned 10% of the time.
     
     @param Vector< IndexedDouble > weightedValues: a Vector of IndexedDouble values, the (int) indexs represent the value that will be returned while the (Float) values represent the weight of choosing that specific index 
     @return returns a random integer from the values Vector, with a probability relative to the values weight
     */
    int getRandomNumberWeighted(Vector< IndexedDouble > weightedValues){
        
        unsigned int N = (unsigned int)weightedValues.size();
        
        if( N == 0 ) return 0;
        if( N == 1 ) return weightedValues[0].index;
        
        //Sort the weighted values by value in ascending order (so the least likely value is first, the second most likely is second, etc...
        sort(weightedValues.begin(),weightedValues.end(),IndexedDouble::sortIndexedDoubleByValueAscending);
        
        //Create the accumulated sum lookup table
        Vector< Float > x(N);
        x[0] = weightedValues[0].value;
        for(unsigned int i=1; i<N; i++){
            x[i] = x[i-1] + weightedValues[i].value;
        }
        
        //Generate a random value between min and the max weighted Float values
        Float randValue = getRandomNumberUniform(0,x[N-1]);
        
        //Find which bin the rand value falls into, return the index of that bin
        for(unsigned int i=0; i<N; i++){
            if( randValue <= x[i] ){
                return weightedValues[ i ].index;
            }
        }
        return 0;
    }
    
    /**
     This function is similar to the getRandomNumberWeighted(Vector< IndexedDouble > weightedValues), with the exception that the user needs
     to sort the weightedValues Vector and create the accumulated lookup table (x). This is useful if you need to call the same function 
     multiple times on the same weightedValues, allowing you to only sort and build the loopup table once.
     
     Gets a random integer from the input Vector. The probability of choosing a specific integer is given by the
     corresponding weight of that value. The weights do not need to sum to 1.
     
     For example, if the input values are: [{1 0.7},{2 0.2}, {3 0.1}], then the 1 value would be randomly returned
     70% of the time, the 2 value returned 20% of the time and the 3 value returned 10% of the time.
     
     @param weightedValues: a sorted Vector of IndexedDouble values, the (int) indexs represent the value that will be returned while the (Float) values represent the weight of choosing that specific index
     @param x: a Vector containing the accumulated lookup table
     @return returns a random integer from the values Vector, with a probability relative to the values weight
     */
    int getRandomNumberWeighted(Vector< IndexedDouble > &weightedValues, VectorFloat &x){
        
        unsigned int N = (unsigned int)weightedValues.size();
        
        if( weightedValues.size() != x.size() ) return 0;
        
        //Generate a random value between min and the max weighted Float values
        Float randValue = getRandomNumberUniform(0,x[N-1]);
        
        //Find which bin the rand value falls into, return the index of that bin
        for(unsigned int i=0; i<N; i++){
            if( randValue <= x[i] ){
                return weightedValues[ i ].index;
            }
        }
        return 0;
    }
    
    /**
	Gets a random Float in the range [minRange maxRange], using a uniform distribution
	
	@param minRange: the minimum value in the range (inclusive)
	@param maxRange: the maximum value in the range (inclusive)
	@return returns a Float in the range [minRange maxRange]
	*/
    inline Float getRandomNumberUniform(Float minRange=0.0,Float maxRange=1.0){
        return (doub()*(maxRange-minRange))+minRange;
    }
    
    /**
	Gets a random Float, using a Gaussian distribution with mu 0 and sigma 1.0
	
	@param mu: the mu parameter for the Gaussian distribution
	@param sigma: the sigma parameter for the Gaussian distribution
	@return returns a Float from the Gaussian distribution controlled by mu and sigma
	*/
    Float getRandomNumberGauss(Float mu=0.0,Float sigma=1.0){
        Float v1,v2,rsq,fac;
        
        if (storedval == 0.){
            do {
                v1=2.0*doub()-1.0;
                v2=2.0*doub()-1.0;
                rsq=v1*v1+v2*v2;
            } while (rsq >= 1.0 || rsq == 0.0);
            fac=sqrt(-2.0*log(rsq)/rsq);
            storedval = v1*fac;
            return mu + sigma*v2*fac;
        } else {
            fac = storedval;
            storedval = 0.;
            return mu + sigma*fac;
        }
    }
    
    /**
	Gets an N-dimensional Vector of random Floats drawn from the uniform distribution set by the minRange and maxRange.
	
	@param numDimensions: the size of the Vector you require
	@param minRange: the minimum value in the range (inclusive)
	@param maxRange: the maximum value in the range (inclusive)
	@return returns a Vector of Floats drawn from the uniform distribution set by the minRange and maxRange
	*/
    VectorFloat getRandomVectorUniform(UINT numDimensions,Float minRange=0.0,Float maxRange=1.0){
        VectorFloat randomValues(numDimensions);
        for(UINT i=0; i<numDimensions; i++){
            randomValues[i] = getRandomNumberUniform(minRange,maxRange);
        }
        return randomValues;
    }
    
    /**
	Gets an N-dimensional Vector of random Floats drawn from the Gaussian distribution controlled by mu and sigma.
	
	@param numDimensions: the size of the Vector you require
	@param mu: the mu parameter for the Gaussian distribution
	@param sigma: the sigma parameter for the Gaussian distribution
	@return returns a Vector of Floats drawn from the Gaussian distribution controlled by mu and sigma
	*/
    VectorFloat getRandomVectorGauss(UINT numDimensions,Float mu=0.0,Float sigma=1.0){
        VectorFloat randomValues(numDimensions);
        for(UINT i=0; i<numDimensions; i++){
            randomValues[i] = getRandomNumberGauss(mu,sigma);
        }
        return randomValues;
    }
    
    /**
     Gets an N-dimensional Vector of random unsigned ints drawn from the range controlled by the start and end range parameters.
     
     @param startRange: indicates the start of the range the random subset will selected from (e.g. 0)
     @param endRange: indicates the end of the range the random subset will selected from (e.g. 100)
     @param subsetSize: controls the size of the Vector returned by the function (e.g. 50
     @return returns a Vector of unsigned ints selected from the
     */
    Vector< unsigned int > getRandomSubset( const unsigned int startRange, const unsigned int endRange, const unsigned int subsetSize ){
        
        unsigned int i = 0;
        const unsigned int rangeSize = endRange - startRange;
        
        grt_assert( rangeSize > 0 );
        grt_assert( endRange > startRange );
        grt_assert( subsetSize <= rangeSize );
        
        Vector< unsigned int > indexs( rangeSize );
        Vector< unsigned int > subset ( subsetSize );
        
        //Fill up the range buffer and the randomly suffle it
        for(i=startRange; i<endRange; i++){
            indexs[i] = i;
        }
        std::random_shuffle(indexs.begin(), indexs.end());
        
        //Select the first X values from the randomly shuffled range buffer as the subset
        for(i=0; i<subsetSize; i++){
            subset[i] = indexs[i];
        }
        
        return subset;
    }
    
private:
    inline unsigned long long int64() {
        u = u * 2862933555777941757LL + 7046029254386353087LL;
        v ^= v >> 17; v ^= v << 31; v ^= v >> 8;
        w = 4294957665U*(w & 0xffffffff) + (w >> 32);
        unsigned long long x = u ^ (u << 21); x ^= x >> 35; x ^= x << 4;
        return (x + v) ^ w;
    }
    inline Float doub() { return 5.42101086242752217E-20 * int64(); }
    inline unsigned int int32() { return (unsigned int)int64(); } 
    
    unsigned long long u;
    unsigned long long v;
    unsigned long long w;
    Float storedval;               //This is for the Gauss Box-Muller 
};

GRT_END_NAMESPACE

#endif //GRT_RANDOM_HEADER
