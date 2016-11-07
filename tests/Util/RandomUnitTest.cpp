#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Random algorithm

// Tests the basic functionality
TEST(Random, SetSeedTest) {

  //Create a new random number generator
  Random random;

  const UINT N = 1000; //Number of iterations in each for loop

  //Set the random seed in a for loop, the random value should be the same every time
  {
    EXPECT_TRUE( random.setSeed( 1 ) );
    const int randomValue = random.getRandomNumberInt( 0, 1000 );
    for(UINT i=0; i<N; i++)
    {
      EXPECT_TRUE( random.setSeed( 1 ) ); //Set the same seed
      EXPECT_EQ( random.getRandomNumberInt( 0, 1000 ), randomValue );
    }
  }

  //Do the same thing, but this time use a different seed everytime
  {
    unsigned long long nextSeed = 0;
    EXPECT_TRUE( random.setSeed( ++nextSeed ) );
    const int randomValue = random.getRandomNumberInt( 0, 1000 );
    float numMatches = 0;
    for(UINT i=0; i<N; i++)
    {
      EXPECT_TRUE( random.setSeed( ++nextSeed ) );
      if( random.getRandomNumberInt( 0, 1000 ) == randomValue )
      {
        numMatches++;
      }
    }
    EXPECT_LT( numMatches, N * 0.5 ); //There should be much less than 50% of matches
  }

   //Do the same thing, but this time don't change the seed
  {
    EXPECT_TRUE( random.setSeed( 1 ) );
    const int randomValue = random.getRandomNumberInt( 0, 1000 );
    float numMatches = 0;
    for(UINT i=0; i<N; i++)
    {
      if( random.getRandomNumberInt( 0, 1000 ) == randomValue )
      {
        numMatches++;
      }
    }
    EXPECT_LT( numMatches, N * 0.5 ); //There should be much less than 50% of matches
  }
}

// Tests the random int functionality
TEST(Random, UniformIntTest) {

  //Create a new random number generator
  Random random;

  //Set the seed so the results are consistent acros unit tests
  EXPECT_TRUE( random.setSeed( 1 ) );

  const UINT N = 10000; //Number of tests
  const int rangeMin = 1000;
  const int rangeMax = 2000;
  const int range = rangeMax-rangeMin;
  VectorFloat histogram( range, 0 );

  int randomValue = 0;
  int index = 0;
  for(UINT i=0; i<N; i++)
  {
    randomValue = random.getRandomNumberInt( rangeMin, rangeMax );
    EXPECT_LT( randomValue, rangeMax );
    EXPECT_GE( randomValue, rangeMin );

    index = randomValue-rangeMin;
    ASSERT_LT( index, range );
    ASSERT_GE( index, 0 );
    histogram[ index ]+= 1.0/N;
  }

  //Compute the histogram min/max difference, the max should be close to the min (as the histogram should be almost equal)
  Float maxValue = histogram.getMaxValue();
  Float minValue = histogram.getMinValue();
  Float diff = maxValue-minValue;
  EXPECT_LT( diff, N*0.01 );
}

// Tests the random float functionality
TEST(Random, UniformRealTest) {

  //Create a new random number generator
  Random random;

  //Set the seed so the results are consistent acros unit tests
  EXPECT_TRUE( random.setSeed( 1 ) );

  const UINT N = 10000; //Number of tests
  const Float rangeMin = 1000;
  const Float rangeMax = 2000;
  const Float range = rangeMax-rangeMin;
  VectorFloat histogram( range, 0 );

  Float randomValue = 0;
  int index = 0;
  for(UINT i=0; i<N; i++)
  {
    randomValue = random.getRandomNumberUniform( rangeMin, rangeMax );
    EXPECT_LT( randomValue, rangeMax );
    EXPECT_GE( randomValue, rangeMin );

    index = (int)floor( randomValue-rangeMin );
    ASSERT_LT( index, range );
    ASSERT_GE( index, 0 );
    histogram[ index ]+= 1.0/N;
  }

  //Compute the histogram min/max difference, the max should be close to the min (as the histogram should be almost equal)
  Float maxValue = histogram.getMaxValue();
  Float minValue = histogram.getMinValue();
  Float diff = maxValue-minValue;
  EXPECT_LT( diff, N*0.01 );
}

// Tests the random normal functionality
TEST(Random, UniformGaussTest) {

  //Create a new random number generator
  Random random;

  //Set the seed so the results are consistent acros unit tests
  EXPECT_TRUE( random.setSeed( 1 ) );

  const UINT N = 10000; //Number of tests
  const Float mu = 10.0;
  const Float sigma = 2.0;
  VectorFloat values( N, 0 );

  for(UINT i=0; i<N; i++)
  {
    values[i] = random.getRandomNumberGauss( mu, sigma );
  }

  //Get the mean and standard deviation from the values vector, this should be very close to the mu, sigma parameters
  Float mean = values.getMean();
  Float stdDev = values.getStdDev();
  EXPECT_LT( mean-mu, 0.01 );
  EXPECT_LT( sigma-stdDev, 0.02 );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
