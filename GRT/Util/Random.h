/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief This file contains the Random class, a useful wrapper for generating cross platform random functions. 
 This includes functions for uniform distributions (both integer and double) and Gaussian distributions.
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

#include "../Util/GRTVersionInfo.h"
#include "Timer.h"

namespace GRT{

class Random{
public:
	/**
	Default constructor. 
	Sets the random seed. If no seed is supplied then the seed will be set using the current system time.
	
	@param unsigned long long seed: sets the current seed, If no seed is supplied then the seed will be set using the current system time
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
	
	@param unsigned long long seed: sets the current seed, If no seed is supplied then the seed will be set using the current system time
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
     Gets a random integer from the vector values. The probability of choosing a specific integer from the
     values vector is given by the corresponding weight in the weights vector. The size of the values
     vector must match the size of the weights vector. The weights do not need to sum to 1.
     
     For example, if the input values are: [1 2 3] and weights are: [0.7 0.2 0.1], then the 1 value would
     be randomly returned 70% of the time, the 2 value returned 20% of the time and the 3 value returned
     10% of the time.
     
     @param const vector< int > &values: a vector containing the N possible values the function can return
     @param const vector< double > &weights: the corresponding weights for the values vector (must be the same size as the values vector)
     @return returns a random integer from the values vector, with a probability relative to the values weight
     */
    int getRandomNumberWeighted(const vector< int > &values,const vector< double > &weights){
        
        if( values.size() != weights.size() ) return 0;
        
        unsigned int N = (unsigned int)values.size();
        vector< IndexedDouble > weightedValues( N );
        for(unsigned int i=0; i<N; i++){
            weightedValues[i].index = values[i];
            weightedValues[i].value = weights[i];
        }
        
        return getRandomNumberWeighted( weightedValues );
    }
    
    /**
     Gets a random integer from the input vector. The probability of choosing a specific integer is given by the 
     corresponding weight of that value. The weights do not need to sum to 1.
     
     For example, if the input values are: [{1 0.7},{2 0.2}, {3 0.1}], then the 1 value would be randomly returned 
     70% of the time, the 2 value returned 20% of the time and the 3 value returned 10% of the time.
     
     @param vector< IndexedDouble > weightedValues: a vector of IndexedDouble values, the (int) indexs represent the value that will be returned while the (double) values represent the weight of choosing that specific index 
     @return returns a random integer from the values vector, with a probability relative to the values weight
     */
    int getRandomNumberWeighted(vector< IndexedDouble > weightedValues){
        
        unsigned int N = (unsigned int)weightedValues.size();
        
        if( N == 0 ) return 0;
        if( N == 1 ) return weightedValues[0].index;
        
        //Sort the weighted values by value in ascending order (so the least likely value is first, the second most likely is second, etc...
        sort(weightedValues.begin(),weightedValues.end(),IndexedDouble::sortIndexedDoubleByValueAscending);
        
        //Create the accumulated sum lookup table
        vector< double > x(N);
        x[0] = weightedValues[0].value;
        for(unsigned int i=1; i<N; i++){
            x[i] = x[i-1] + weightedValues[i].value;
        }
        
        //Generate a random value between min and the max weighted double values
        double randValue = getRandomNumberUniform(0,x[N-1]);
        
        //Find which bin the rand value falls into, return the index of that bin
        for(unsigned int i=0; i<N; i++){
            if( randValue <= x[i] ){
                return weightedValues[ i ].index;
            }
        }
        return 0;
    }
    
    /**
     This function is similar to the getRandomNumberWeighted(vector< IndexedDouble > weightedValues), with the exception that the user needs
     to sort the weightedValues vector and create the accumulated lookup table (x). This is useful if you need to call the same function 
     multiple times on the same weightedValues, allowing you to only sort and build the loopup table once.
     
     Gets a random integer from the input vector. The probability of choosing a specific integer is given by the
     corresponding weight of that value. The weights do not need to sum to 1.
     
     For example, if the input values are: [{1 0.7},{2 0.2}, {3 0.1}], then the 1 value would be randomly returned
     70% of the time, the 2 value returned 20% of the time and the 3 value returned 10% of the time.
     
     @param vector< IndexedDouble > &weightedValues: a sorted vector of IndexedDouble values, the (int) indexs represent the value that will be returned while the (double) values represent the weight of choosing that specific index
     @param vector< double > &x: a vector containing the accumulated lookup table
     @return returns a random integer from the values vector, with a probability relative to the values weight
     */
    int getRandomNumberWeighted(vector< IndexedDouble > &weightedValues, vector< double > &x){
        
        unsigned int N = (unsigned int)weightedValues.size();
        
        if( weightedValues.size() != x.size() ) return 0;
        
        //Generate a random value between min and the max weighted double values
        double randValue = getRandomNumberUniform(0,x[N-1]);
        
        //Find which bin the rand value falls into, return the index of that bin
        for(unsigned int i=0; i<N; i++){
            if( randValue <= x[i] ){
                return weightedValues[ i ].index;
            }
        }
        return 0;
    }
    
    /**
	Gets a random double in the range [minRange maxRange], using a uniform distribution
	
	@param double minRange: the minimum value in the range (inclusive)
	@param double maxRange: the maximum value in the range (inclusive)
	@return returns a double in the range [minRange maxRange]
	*/
    inline double getRandomNumberUniform(double minRange=0.0,double maxRange=1.0){
        return (doub()*(maxRange-minRange))+minRange;
    }
    
    /**
	Gets a random double, using a Gaussian distribution with mu 0 and sigma 1.0
	
	@param double mu: the mu parameter for the Gaussian distribution
	@param double sigma: the sigma parameter for the Gaussian distribution
	@return returns a double from the Gaussian distribution controlled by mu and sigma
	*/
    double getRandomNumberGauss(double mu=0.0,double sigma=1.0){
        double v1,v2,rsq,fac;
        
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
	Gets an N-dimensional vector of random doubles drawn from the uniform distribution set by the minRange and maxRange.
	
	@param UINT numDimensions: the size of the vector you require
	@param double minRange: the minimum value in the range (inclusive)
	@param double maxRange: the maximum value in the range (inclusive)
	@return returns a vector of doubles drawn from the uniform distribution set by the minRange and maxRange
	*/
    VectorDouble getRandomVectorUniform(UINT numDimensions,double minRange=0.0,double maxRange=1.0){
        VectorDouble randomValues(numDimensions);
        for(UINT i=0; i<numDimensions; i++){
            randomValues[i] = getRandomNumberUniform(minRange,maxRange);
        }
        return randomValues;
    }
    
    /**
	Gets an N-dimensional vector of random doubles drawn from the Gaussian distribution controlled by mu and sigma.
	
	@param UINT numDimensions: the size of the vector you require
	@param double mu: the mu parameter for the Gaussian distribution
	@param double sigma: the sigma parameter for the Gaussian distribution
	@return returns a vector of doubles drawn from the Gaussian distribution controlled by mu and sigma
	*/
    VectorDouble getRandomVectorGauss(UINT numDimensions,double mu=0.0,double sigma=1.0){
        VectorDouble randomValues(numDimensions);
        for(UINT i=0; i<numDimensions; i++){
            randomValues[i] = getRandomNumberGauss(mu,sigma);
        }
        return randomValues;
    }
    
private:
    inline unsigned long long int64() {
        u = u * 2862933555777941757LL + 7046029254386353087LL;
        v ^= v >> 17; v ^= v << 31; v ^= v >> 8;
        w = 4294957665U*(w & 0xffffffff) + (w >> 32);
        unsigned long long x = u ^ (u << 21); x ^= x >> 35; x ^= x << 4;
        return (x + v) ^ w;
    }
    inline double doub() { return 5.42101086242752217E-20 * int64(); }
    inline unsigned int int32() { return (unsigned int)int64(); } 
    
    unsigned long long u;
    unsigned long long v;
    unsigned long long w;
    double storedval;               //This is for the Gauss Box-Muller 
};

}; //End of namespace GRT

#endif //GRT_RANDOM_HEADER