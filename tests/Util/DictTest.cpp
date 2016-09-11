#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Typedefs

// Tests the basic functionality
TEST(Dict, DictTest) {

  //Create a new dictionary
  Dict dict;

  //Create some values to add to the dictionary
  const int apple = 1;
  const int orange = 2;
  const int melon = 3;
  const Float pi = 3.14;
  VectorFloat buf(3);
  buf[0] = 1; buf[1] = 2; buf[2] = 3;
  int expectedSize = 0;

  //Check the size, it should be zero
  EXPECT_TRUE( dict.getSize() == expectedSize );

  //Add some key-value pairs to the dictionary
  EXPECT_TRUE( dict.add( "apple", apple ) );
  EXPECT_TRUE( dict.getSize() == ++expectedSize );
  EXPECT_TRUE( dict.add( "orange", orange ) );
  EXPECT_TRUE( dict.getSize() == ++expectedSize );
  EXPECT_TRUE( dict.add( "melon", melon ) );
  EXPECT_TRUE( dict.getSize() == ++expectedSize );
  EXPECT_TRUE( dict.add( "pi", pi ) );
  EXPECT_TRUE( dict.getSize() == ++expectedSize );
  EXPECT_TRUE( dict.add( "pi", pi ) ); //Add it twice, the first value will be overwritten
  EXPECT_TRUE( dict.getSize() == expectedSize );
  EXPECT_FALSE( dict.add( "pi", pi, false ) ); //Try and add it, but disable overwrites
  EXPECT_TRUE( dict.getSize() == expectedSize );
  EXPECT_TRUE( dict.add( "buf", buf ) );
  EXPECT_TRUE( dict.getSize() == ++expectedSize );

  //Remove some values
  EXPECT_TRUE( dict.remove( "orange" ) );
  EXPECT_TRUE( dict.getSize() == --expectedSize );
  EXPECT_FALSE( dict.remove( "orange" ) ); //Try and remove the value a second time
  EXPECT_TRUE( dict.getSize() == expectedSize );
  EXPECT_FALSE( dict.remove( "pear" ) ); //Try and remove a value that does not exist
  EXPECT_TRUE( dict.getSize() == expectedSize );

  //Test some keys exist
  EXPECT_TRUE( dict.exists( "apple" ) );
  EXPECT_TRUE( dict.exists( "pi" ) );
  EXPECT_FALSE( dict.exists( "orange" ) );

  //Test the getter
  EXPECT_EQ( dict.get< int >( "apple" ), apple );
  EXPECT_EQ( dict.get< Float >( "pi" ), pi );

  //Test the reference update
  int &v = dict.get< int >( "apple" );
  v++;
  EXPECT_EQ( dict.get< int >( "apple" ), apple+1 );

  //Test the vector
  VectorFloat vec = dict.get< VectorFloat >( "buf" );
  EXPECT_TRUE( buf.getSize() == vec.getSize() );
  for(UINT i=0; i<buf.getSize(); i++){
    EXPECT_TRUE( buf[i] == vec[i] );
  }

  //Test the keys
  Vector< std::string > keys = dict.getKeys();
  EXPECT_EQ( keys.getSize(), dict.getSize() );

  //Test the setter
  EXPECT_TRUE( dict.set( "pi", 3.14159 ) );
  EXPECT_FALSE( dict.set( "foo", 3.14159 ) ); //This wil fail, as foo does not exist in the dictionary

  //Test the () operator
  int x = 0;
  EXPECT_TRUE( dict("melon", x) );
  EXPECT_EQ( x, melon );

  //This should return false
  EXPECT_FALSE( dict("pear", x) );

  //Test the copy constructor
  Dict d2( dict );

  EXPECT_EQ( dict.getSize(), d2.getSize() );
  EXPECT_EQ( dict.getSize(), expectedSize );

  //The values should match
  EXPECT_TRUE( dict.get< int >( "apple" ) == d2.get< int >( "apple" ) );

  //Change something in the original dict, test that d2 has not changed
  EXPECT_TRUE( dict.set( "apple", 5 ) );

  //The values should now not match
  EXPECT_FALSE( dict.get< int >( "apple" ) == d2.get< int >( "apple" ) );

  //Clear all the values
  EXPECT_TRUE( dict.clear() );
  EXPECT_TRUE( dict.getSize() == 0 );

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
