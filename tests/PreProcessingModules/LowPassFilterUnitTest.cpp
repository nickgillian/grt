#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT LowPassFilter module
const std::string modelFilename = "lpf_model.grt";
Float ROUND( const Float x ){ return roundf(x*100.0)/100.0; } //Round the value to 3 decimal places
Float GET_FILTER_FACTOR( const Float freq, const Float delta ){ return delta / ((ONE_OVER_TWO_PI/freq) + delta); }

// Tests the default constructor
TEST(LowPassFilter, TestDefaultConstructor) {
  
  LowPassFilter lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf.getId() == LowPassFilter::getId() );

  //Check the lpf is not trained
  EXPECT_TRUE( !lpf.getTrained() );
}

// Tests the copy constructor
TEST(LowPassFilter, TestCopyConstructor) {
  
  LowPassFilter lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf.getId() == LowPassFilter::getId() );

  //Check the lpf is not trained
  EXPECT_TRUE( !lpf.getTrained() );

  LowPassFilter lpf2( lpf );

  //Check the id's matches
  EXPECT_TRUE( lpf2.getId() == LowPassFilter::getId() );

  //Check the lpf is not trained
  EXPECT_TRUE( !lpf2.getTrained() );
}

// Tests the equals operator
TEST(LowPassFilter, TestEqualsOperator) {
  
  LowPassFilter lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf.getId() == LowPassFilter::getId() );

  //Check the lpf is not trained
  EXPECT_TRUE( !lpf.getTrained() );

  LowPassFilter lpf2 = lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf2.getId() == LowPassFilter::getId() );

  //Check the lpf is not trained
  EXPECT_TRUE( !lpf2.getTrained() );
}

// Tests the gain getter/setter
TEST(LowPassFilter, TestGainGetSet) {
  
  LowPassFilter lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf.getId() == LowPassFilter::getId() );

  //Set the gain with a valid value
  EXPECT_TRUE( lpf.setGain( 0.9 ) );
  EXPECT_EQ( lpf.getGain(), 0.9 );

  //Set the gain with a non valid value
  EXPECT_FALSE( lpf.setGain( -1 ) );
  EXPECT_EQ( lpf.getGain(), 0.9 ); //The gain should not have changed

  //Set the gain with a valid value
  EXPECT_TRUE( lpf.setGain( 1.1 ) );
  EXPECT_EQ( lpf.getGain(), 1.1 ); //The gain should now have changed
}

// Tests the filter factor getter/setter
TEST(LowPassFilter, TestFilterFactorGetSet) {
  
  LowPassFilter lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf.getId() == LowPassFilter::getId() );

  //Set the gain with a valid value
  EXPECT_TRUE( lpf.setFilterFactor( 0.9 ) );
  EXPECT_EQ( lpf.getFilterFactor(), 0.9 );

  //Set the gain with a non valid value
  EXPECT_FALSE( lpf.setFilterFactor( -1 ) );
  EXPECT_EQ( lpf.getFilterFactor(), 0.9 ); //The gain should not have changed

  //Set the gain with a non valid value
  EXPECT_FALSE( lpf.setFilterFactor( 1.1 ) );
  EXPECT_EQ( lpf.getFilterFactor(), 0.9 ); //The gain should not have changed
}

// Tests the filter factor setter
TEST(LowPassFilter, TestCutoffFrequencySet) {
  
  LowPassFilter lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf.getId() == LowPassFilter::getId() );

  const Float delta = 1.0 / 500.0f; //Set a delta for 500Hz
  const Float expectedFilterFactor = delta / ((ONE_OVER_TWO_PI/10.0) + delta);

  //Set the gain with a valid value
  EXPECT_TRUE( lpf.setCutoffFrequency( 10, delta ) );
  EXPECT_EQ( lpf.getFilterFactor(), expectedFilterFactor );

  //Set the gain with a non valid value
  EXPECT_FALSE( lpf.setCutoffFrequency( -1, delta ) );
  EXPECT_EQ( lpf.getFilterFactor(), expectedFilterFactor ); //The gain should not have changed

  //Set the gain with a non valid value
  EXPECT_FALSE( lpf.setCutoffFrequency( 0, delta ) );
  EXPECT_EQ( lpf.getFilterFactor(), expectedFilterFactor ); //The gain should not have changed

  //Set the gain with a non valid value
  EXPECT_FALSE( lpf.setCutoffFrequency( 10, 0 ) );
  EXPECT_EQ( lpf.getFilterFactor(), expectedFilterFactor ); //The gain should not have changed
}

// Tests the save/load functions
TEST(LowPassFilter, TestSaveLoad) {
  
  LowPassFilter lpf;

  //Check the id's matches
  EXPECT_TRUE( lpf.getId() == LowPassFilter::getId() );

  const Float delta = 1.0 / 500.0f; //Set a delta for 500Hz
  const Float expectedFilterFactor = delta / ((ONE_OVER_TWO_PI/10.0) + delta);

  //Set the gain with a valid value
  EXPECT_TRUE( lpf.setCutoffFrequency( 10, delta ) );
  EXPECT_EQ( lpf.getFilterFactor(), expectedFilterFactor );

  EXPECT_TRUE( lpf.save("lpf_unit_test_model.grt") );

  EXPECT_TRUE( lpf.clear() );

  EXPECT_TRUE( lpf.load("lpf_unit_test_model.grt") );
  EXPECT_EQ( ROUND( lpf.getFilterFactor() ), ROUND( expectedFilterFactor ) ); //Due to differences in how things are load from the file, we need to round the expected value
}

// Tests the filter functions
TEST(LowPassFilter, TestFilter) {
  
  LowPassFilter lpf_1( 0.9 );
  LowPassFilter lpf_2( 0.999 );

  const UINT numSeconds = 5;                         //The number of seconds of data we want to generate
  const Float sampleRate = 1000.0f;
  const Float delta = 1.0 / sampleRate; //Set a delta for 1000Hz smaple rate
  const Float freq = 1.0;                           //Stores the frequency
  Float t = 0;                                       //This keeps track of the time

  Float sum_1 = 0;
  Float sum_2 = 0;
  Random random;
  for(UINT i=0; i<numSeconds*1000; i++){
        
        //Generate the signal
        Float signal = sin( t * TWO_PI * freq ) + random.getRandomNumberGauss( 0, 0.1 );
        
        //Filter the signal using the two filters
        Float filteredValue_1 = lpf_1.filter( signal );
        Float filteredValue_2 = lpf_2.filter( signal );
        
        //Update the t
        t += delta;

        sum_1 += fabs( filteredValue_1 );
        sum_2 += fabs( filteredValue_2 );
    }

    EXPECT_GT( sum_1, sum_2  ); //The gain for lpf 1 should be larger than that for 2 (as lpf 2 has a more aggressive filter)
  
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
