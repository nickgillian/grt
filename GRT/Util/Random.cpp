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
#include "Random.h"

GRT_BEGIN_NAMESPACE

#if GRT_USE_CXX11_RANDOM_ALGO
Random::Random( ) : uniformRealDistribution(0.0,1.0), normalDistribution(0.0,1.0){}
#else
Random::Random(unsigned long long seed):v(4101842887655102017LL), w(1), storedval(0.0){
    if( seed == 0 ){
        Timer t;
        seed = (unsigned long long)t.getSystemTime();
    }
    setSeed( seed );
}
#endif

Random::~Random(){}
    
bool Random::setSeed(const unsigned long long seed_){
#if GRT_USE_CXX11_RANDOM_ALGO
    generator.seed( seed_ );
#else
    unsigned long long seed = seed_;
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
#endif
    return true;
}

int Random::getRandomNumberInt(int minRange,int maxRange){
#if GRT_USE_CXX11_RANDOM_ALGO
    std::uniform_int_distribution< int > uniformIntDistribution(minRange, maxRange-1);
    return uniformIntDistribution( generator );
#else
    return int( floor(getRandomNumberUniform(minRange,maxRange)) );
#endif
}
    
int Random::getRandomNumberWeighted(const Vector< int > &values,const VectorFloat &weights){
    
    if( values.size() != weights.size() ) return 0;
    
    unsigned int N = (unsigned int)values.size();
    Vector< IndexedDouble > weightedValues( N );
    for(unsigned int i=0; i<N; i++){
        weightedValues[i].index = values[i];
        weightedValues[i].value = weights[i];
    }
    
    return getRandomNumberWeighted( weightedValues );
}

int Random::getRandomNumberWeighted(Vector< IndexedDouble > weightedValues){
    
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

int Random::getRandomNumberWeighted(Vector< IndexedDouble > &weightedValues, VectorFloat &x){
    
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

Float Random::getRandomNumberUniform(Float minRange,Float maxRange){
#if GRT_USE_CXX11_RANDOM_ALGO
    Float r = uniformRealDistribution( generator );
#else
    Float r = doub();
#endif
    return (r*(maxRange-minRange))+minRange;
}

Float Random::getRandomNumberGauss(Float mu,Float sigma){
#if GRT_USE_CXX11_RANDOM_ALGO
    return mu + (normalDistribution( generator )*sigma);
#else
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
#endif
}

VectorFloat Random::getRandomVectorUniform(UINT numDimensions,Float minRange,Float maxRange){
    VectorFloat randomValues(numDimensions);
    for(UINT i=0; i<numDimensions; i++){
        randomValues[i] = getRandomNumberUniform(minRange,maxRange);
    }
    return randomValues;
}

VectorFloat Random::getRandomVectorGauss(UINT numDimensions,Float mu,Float sigma){
    VectorFloat randomValues(numDimensions);
    for(UINT i=0; i<numDimensions; i++){
        randomValues[i] = getRandomNumberGauss(mu,sigma);
    }
    return randomValues;
}

Vector< unsigned int > Random::getRandomSubset( const unsigned int startRange, const unsigned int endRange, const unsigned int subsetSize ){
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

GRT_END_NAMESPACE
