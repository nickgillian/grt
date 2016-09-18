#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT CommandLineParser

// Tests the square function
TEST(CommandLineParser, BasicGetterSetters) {
  CommandLineParser parser;

  //Add a new option
  EXPECT_TRUE( parser.addOption( "--apple", "apple", (int)1 ) );
  EXPECT_TRUE( parser.addOption( "--orange", "orange", (int)2 ) );
  EXPECT_TRUE( parser.addOption( "--kiwi", "kiwi" ) ); //Don't supply a default valueInt for kiwi
  EXPECT_TRUE( parser.addOption( "--verbose", "verbose" ) ); //Don't supply a default valueInt
  EXPECT_TRUE( parser.addOption( "--pi", "pi", (float)3.14 ) );

  //Try and add an option that already exists (which should fail)
  EXPECT_TRUE( !parser.addOption( "--orange", "orange", (int)2 ) );

  //The number of options should be three at this point
  EXPECT_TRUE( parser.getNumOptions() == 5 );

  //Try and get the valueInt for apple, this should return true (as nothing has
  //been parsed yet, but a default valueInt was provided) and give the default valueInt 1
  int valueInt = 0;
  float valueFloat = 0;
  EXPECT_TRUE( parser.get("apple",valueInt) );
  EXPECT_TRUE( valueInt == 1 );

  //Try the same for orange
  EXPECT_TRUE( parser.get("orange",valueInt) );
  EXPECT_TRUE( valueInt == 2 );
  
  //Try the same for kiwi, this should fail because no default valueInt was set
  EXPECT_TRUE( !parser.get("kiwi",valueInt) );
  EXPECT_TRUE( valueInt == 2 ); //The valueInt should not be touched
  
  //We haven't parsed anything yet, so these should all return false
  EXPECT_TRUE( !parser.getOptionParsed("apple") );
  EXPECT_TRUE( !parser.getOptionParsed("orange") );
  EXPECT_TRUE( !parser.getOptionParsed("kiwi") );
  EXPECT_TRUE( !parser.getOptionParsed("verbose") );
  
  //We haven't parsed anything yet, so these should all return false
  EXPECT_TRUE( parser.getValidId("apple") );
  EXPECT_TRUE( parser.getValidId("orange") );
  EXPECT_TRUE( parser.getValidId("kiwi") );
  EXPECT_TRUE( !parser.getValidId("plum") );
  EXPECT_TRUE( parser.getValidId("verbose") );

  //Give the parser some fake data to parse
  int idx = 1; //Start at 1 because the defauly command line argument will have the application as the first arg
  int argc = 1 + 7;
  char *argv[argc];
  argv[idx++] = (char*)"--apple";
  argv[idx++] = (char*)"5";
  argv[idx++] = (char*)"--kiwi";
  argv[idx++] = (char*)"15";
  argv[idx++] = (char*)"--pi";
  argv[idx++] = (char*)"3.1415";
  argv[idx++] = (char*)"--verbose"; //note, this option doesn't get a second arg

  EXPECT_TRUE( parser.parse( argc, argv ) );
  
  EXPECT_TRUE( parser.getOptionParsed("apple") );
  EXPECT_TRUE( parser.getOptionParsed("kiwi") );
  EXPECT_TRUE( !parser.getOptionParsed("orange") );
  EXPECT_TRUE( !parser.getOptionParsed("plum") );
  EXPECT_TRUE( parser.getOptionParsed("pi") );
  EXPECT_TRUE( parser.getOptionParsed("verbose") );
  
  EXPECT_TRUE( parser.get("apple",valueInt) );
  EXPECT_TRUE( valueInt == 5 );
  
  EXPECT_TRUE( parser.get("kiwi",valueInt) );
  EXPECT_TRUE( valueInt == 15 );
  
  EXPECT_TRUE( parser.get("orange",valueInt) );
  EXPECT_TRUE( valueInt == 2 );
  
  EXPECT_TRUE( parser.get("pi",valueFloat) );
  EXPECT_TRUE( fabs( valueFloat-3.1415f) < 0.01f );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
