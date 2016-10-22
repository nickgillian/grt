#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT LowPassFilter module
const std::string modelFilename = "lpf_model.grt";

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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
