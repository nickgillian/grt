#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT HighPassFilter module
const std::string modelFilename = "hpf_model.grt";
Float ROUND( const Float x ){ return roundf(x*100.0)/100.0; } //Round the value to 3 decimal places
Float GET_FILTER_FACTOR( const Float freq, const Float delta ){ Float RC=(ONE_OVER_TWO_PI/freq); return RC/(RC+delta); }

// Tests the default constructor
TEST(HighPassFilter, TestDefaultConstructor) {
  
  HighPassFilter hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf.getId() == HighPassFilter::getId() );

  //Check the hpf is not trained
  EXPECT_TRUE( !hpf.getTrained() );
}

// Tests the copy constructor
TEST(HighPassFilter, TestCopyConstructor) {
  
  HighPassFilter hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf.getId() == HighPassFilter::getId() );

  //Check the hpf is not trained
  EXPECT_TRUE( !hpf.getTrained() );

  HighPassFilter hpf2( hpf );

  //Check the id's matches
  EXPECT_TRUE( hpf2.getId() == HighPassFilter::getId() );

  //Check the hpf is not trained
  EXPECT_TRUE( !hpf2.getTrained() );
}

// Tests the equals operator
TEST(HighPassFilter, TestEqualsOperator) {
  
  HighPassFilter hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf.getId() == HighPassFilter::getId() );

  //Check the hpf is not trained
  EXPECT_TRUE( !hpf.getTrained() );

  HighPassFilter hpf2 = hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf2.getId() == HighPassFilter::getId() );

  //Check the hpf is not trained
  EXPECT_TRUE( !hpf2.getTrained() );
}

// Tests the gain getter/setter
TEST(HighPassFilter, TestGainGetSet) {
  
  HighPassFilter hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf.getId() == HighPassFilter::getId() );

  //Set the gain with a valid value
  EXPECT_TRUE( hpf.setGain( 0.9 ) );
  EXPECT_EQ( hpf.getGain(), 0.9 );

  //Set the gain with a non valid value
  EXPECT_FALSE( hpf.setGain( -1 ) );
  EXPECT_EQ( hpf.getGain(), 0.9 ); //The gain should not have changed

  //Set the gain with a valid value
  EXPECT_TRUE( hpf.setGain( 1.1 ) );
  EXPECT_EQ( hpf.getGain(), 1.1 ); //The gain should now have changed
}

// Tests the filter factor getter/setter
TEST(HighPassFilter, TestFilterFactorGetSet) {
  
  HighPassFilter hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf.getId() == HighPassFilter::getId() );

  //Set the gain with a valid value
  EXPECT_TRUE( hpf.setFilterFactor( 0.9 ) );
  EXPECT_EQ( hpf.getFilterFactor(), 0.9 );

  //Set the gain with a non valid value
  EXPECT_FALSE( hpf.setFilterFactor( -1 ) );
  EXPECT_EQ( hpf.getFilterFactor(), 0.9 ); //The gain should not have changed

  //Set the gain with a non valid value
  EXPECT_FALSE( hpf.setFilterFactor( 1.1 ) );
  EXPECT_EQ( hpf.getFilterFactor(), 0.9 ); //The gain should not have changed
}

// Tests the filter factor setter
TEST(HighPassFilter, TestCutoffFrequencySet) {
  
  HighPassFilter hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf.getId() == HighPassFilter::getId() );

  const Float delta = 1.0 / 500.0f; //Set a delta for 500Hz

  //Set the gain with a valid value
  EXPECT_TRUE( hpf.setCutoffFrequency( 10, delta ) );
  EXPECT_EQ( hpf.getFilterFactor(), GET_FILTER_FACTOR( 10.0, delta ) );

  //Set the gain with a non valid value
  EXPECT_FALSE( hpf.setCutoffFrequency( -1, delta ) );
  EXPECT_EQ( hpf.getFilterFactor(), GET_FILTER_FACTOR( 10.0, delta ) ); //The gain should not have changed

  //Set the gain with a non valid value
  EXPECT_FALSE( hpf.setCutoffFrequency( 0, delta ) );
  EXPECT_EQ( hpf.getFilterFactor(), GET_FILTER_FACTOR( 10.0, delta ) ); //The gain should not have changed

  //Set the gain with a non valid value
  EXPECT_FALSE( hpf.setCutoffFrequency( 10, 0 ) );
  EXPECT_EQ( hpf.getFilterFactor(), GET_FILTER_FACTOR( 10.0, delta ) ); //The gain should not have changed
}

// Tests the save/load functions
TEST(HighPassFilter, TestSaveLoad) {
  
  HighPassFilter hpf;

  //Check the id's matches
  EXPECT_TRUE( hpf.getId() == HighPassFilter::getId() );

  const Float delta = 1.0 / 500.0f; //Set a delta for 500Hz

  //Set the gain with a valid value
  EXPECT_TRUE( hpf.setCutoffFrequency( 10, delta ) );
  EXPECT_EQ( hpf.getFilterFactor(), GET_FILTER_FACTOR( 10.0, delta ) );

  EXPECT_TRUE( hpf.save("hpf_unit_test_model.grt") );

  EXPECT_TRUE( hpf.clear() );

  EXPECT_TRUE( hpf.load("hpf_unit_test_model.grt") );
  EXPECT_EQ( ROUND( hpf.getFilterFactor() ), ROUND( GET_FILTER_FACTOR( 10.0, delta ) ) ); //Due to differences in how things are load from the file, we need to round the expected value
}

// Tests the filter functions
TEST(HighPassFilter, TestFilter) {
  
  HighPassFilter hpf_1;
  HighPassFilter hpf_2;

  const UINT numSeconds = 5;                         //The number of seconds of data we want to generate
  const Float sampleRate = 1000.0f;
  const Float delta = 1.0 / sampleRate; //Set a delta for 1000Hz smaple rate
  const Float freq = 0.1;                           //Stores the frequency
  Float t = 0;                                       //This keeps track of the time

  EXPECT_TRUE( hpf_1.setCutoffFrequency( 2, delta ) );
  EXPECT_TRUE( hpf_2.setCutoffFrequency( 5, delta ) );

  std::fstream file;
  file.open( "hpf.log", std::fstream::out );

  Float sum_1 = 0;
  Float sum_2 = 0;
  Random random;
  for(UINT i=0; i<numSeconds*1000; i++){
        
        //Generate the signal
        Float signal = sin( t * TWO_PI * freq ) + random.getRandomNumberGauss( 0, 0.1 );
        
        //Filter the signal using the two filters
        Float filteredValue_1 = hpf_1.filter( signal );
        Float filteredValue_2 = hpf_2.filter( signal );
        
        //Update the t
        t += delta;

        sum_1 += fabs( filteredValue_1 );
        sum_2 += fabs( filteredValue_2 );

        file << signal << "," << filteredValue_1 << "," << filteredValue_2 << std::endl;
    }

    file.close();

    EXPECT_GT( sum_1, sum_2  );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
