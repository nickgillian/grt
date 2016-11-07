#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Logs

// Tests the Log base class
TEST(Logging, LogBaseClassConstructor) {
  Log log("log-test");

  std::string stringValue = "hi";
  Float floatValue = 3.14;
  int intValue = -1;

  log << "this is a string: " << stringValue << " and a float: " << floatValue << " and an int: " << intValue << std::endl;
}

// Tests the Log base class
TEST(Logging, LogBaseClassCopyConstructor) {
  Log log("log-test");

  std::string stringValue = "hi";
  Float floatValue = 3.14;
  int intValue = -1;

  log << "this is a string: " << stringValue << " and a float: " << floatValue << " and an int: " << intValue << std::endl;

  Log log2( log );

  //Check that the keys match
  EXPECT_TRUE( log.getKey() == log2.getKey() );

  //Log2 last message should match log 1 last message at this time
  EXPECT_TRUE( log.getLastMessage() == log2.getLastMessage() );

  log2 << "this is a log from the log2!" << std::endl;

  std::cout << "log last message: " << log.getLastMessage() << std::endl;

  //The last messages should no longer match
  EXPECT_TRUE( log.getLastMessage() != log2.getLastMessage() );
}

// Tests the Log base class
TEST(Logging, LogBaseClassEqualsOperator) {
  Log log("log-test");

  std::string stringValue = "hi";
  Float floatValue = 3.14;
  int intValue = -1;

  log << "this is a string: " << stringValue << " and a float: " << floatValue << " and an int: " << intValue << std::endl;

  Log log2;
  log2 = log;

  //Check that the keys match
  EXPECT_TRUE( log.getKey() == log2.getKey() );

  //Log2 last message should match log 1 last message at this time
  EXPECT_TRUE( log.getLastMessage() == log2.getLastMessage() );

  log2 << "this is a log from the log2!" << std::endl;

  //The last messages should no longer match
  EXPECT_TRUE( log.getLastMessage() != log2.getLastMessage() );
}

// Tests the Log base class getters/setters
TEST(Logging, LogBaseClassGetterSetter) {
	const std::string key = "log-test";
	Log log( key );

	//Test the key gettter/setter
	EXPECT_TRUE( log.getKey() == key );
	const std::string newKey = "foo"; 
	EXPECT_TRUE( log.setKey( newKey ) );
	EXPECT_TRUE( log.getKey() == newKey );

	//Test the last message getter/setter
	const std::string testString = "This is a test string";
	log << testString << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString );
	const std::string testString2 = "This is another test string";
	log << testString2 << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Test the instance logging enable/disable getter/setter
	EXPECT_TRUE( log.setLoggingEnabled( true ) );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( log.getLoggingEnabled() == false );
	EXPECT_TRUE( Log::setLoggingEnabled( false ) );
	EXPECT_TRUE( Log::getLoggingEnabled() == false );
	EXPECT_TRUE( Log::setLoggingEnabled( true ) );
	EXPECT_TRUE( Log::getLoggingEnabled() == true );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( Log::getLoggingEnabled() == false );

	//Logging is off so we should NOT see this message
	log << "LOGGING IS OFF, WE SHOULD NOT SEE THIS MESSAGE" << std::endl;

	//The last message should not have changed
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Create a new log instance
	Log log2( "another-log" );

	//Logging should be disabled for log2 as well (as the logging is global across all instances)
	EXPECT_TRUE( log2.getLoggingEnabled() == false );

	//Enable logging on the first instance
	EXPECT_TRUE( log.setLoggingEnabled( true ) );

	//Logging should now be enabled across all instances
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log2.getLoggingEnabled() == true );
}

// Tests the InfoLog class getters/setters
TEST(Logging, InfoLogBaseClassGetterSetter) {
	const std::string key = "log-test";
	InfoLog log( key );

	//Test the key gettter/setter
	EXPECT_TRUE( log.getKey() == key );
	const std::string newKey = "foo"; 
	EXPECT_TRUE( log.setKey( newKey ) );
	EXPECT_TRUE( log.getKey() == newKey );

	//Test the last message getter/setter
	const std::string testString = "This is a test string";
	log << testString << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString );
	const std::string testString2 = "This is another test string";
	log << testString2 << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Test the instance logging enable/disable getter/setter
	EXPECT_TRUE( log.setLoggingEnabled( true ) );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( log.getLoggingEnabled() == false );
	EXPECT_TRUE( InfoLog::setLoggingEnabled( false ) );
	EXPECT_TRUE( InfoLog::getLoggingEnabled() == false );
	EXPECT_TRUE( InfoLog::setLoggingEnabled( true ) );
	EXPECT_TRUE( InfoLog::getLoggingEnabled() == true );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( InfoLog::getLoggingEnabled() == false );

	//Logging is off so we should NOT see this message
	log << "LOGGING IS OFF, WE SHOULD NOT SEE THIS MESSAGE" << std::endl;

	//The last message should not have changed
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Create a new log instance
	InfoLog log2( "another-log" );

	//Logging should be disabled for log2 as well (as the logging is global across all instances)
	EXPECT_TRUE( log2.getLoggingEnabled() == false );

	//Enable logging on the first instance
	EXPECT_TRUE( log.setLoggingEnabled( true ) );

	//Logging should now be enabled across all instances
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log2.getLoggingEnabled() == true );

	//Create a new log instance and test the copy constructor
	InfoLog log3( log2 );

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log2.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log2.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log2.getLoggingEnabled() );

	//Create a new log instance and test the equals operator
	InfoLog log4;

	log4 = log3;

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log4.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log4.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log4.getLoggingEnabled() );
}

// Tests the WarningLog class getters/setters
TEST(Logging, WarningLogBaseClassGetterSetter) {
	const std::string key = "warning-test";
	WarningLog log( key );

	//Test the key gettter/setter
	EXPECT_TRUE( log.getKey() == key );
	const std::string newKey = "foo"; 
	EXPECT_TRUE( log.setKey( newKey ) );
	EXPECT_TRUE( log.getKey() == newKey );

	//Test the last message getter/setter
	const std::string testString = "This is a test string";
	log << testString << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString );
	const std::string testString2 = "This is another test string";
	log << testString2 << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Test the instance logging enable/disable getter/setter
	EXPECT_TRUE( log.setLoggingEnabled( true ) );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( log.getLoggingEnabled() == false );
	EXPECT_TRUE( WarningLog::setLoggingEnabled( false ) );
	EXPECT_TRUE( WarningLog::getLoggingEnabled() == false );
	EXPECT_TRUE( WarningLog::setLoggingEnabled( true ) );
	EXPECT_TRUE( WarningLog::getLoggingEnabled() == true );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( WarningLog::getLoggingEnabled() == false );

	//Logging is off so we should NOT see this message
	log << "LOGGING IS OFF, WE SHOULD NOT SEE THIS MESSAGE" << std::endl;

	//The last message should not have changed
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Create a new log instance
	WarningLog log2( "another-log" );

	//Logging should be disabled for log2 as well (as the logging is global across all instances)
	EXPECT_TRUE( log2.getLoggingEnabled() == false );

	//Enable logging on the first instance
	EXPECT_TRUE( log.setLoggingEnabled( true ) );

	//Logging should now be enabled across all instances
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log2.getLoggingEnabled() == true );

	//Create a new log instance and test the copy constructor
	WarningLog log3( log2 );

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log2.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log2.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log2.getLoggingEnabled() );

	//Create a new log instance and test the equals operator
	WarningLog log4;

	log4 = log3;

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log4.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log4.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log4.getLoggingEnabled() );
}

// Tests the ErrorLog class getters/setters
TEST(Logging, ErrorLogBaseClassGetterSetter) {
	const std::string key = "error-test";
	ErrorLog log( key );

	//Test the key gettter/setter
	EXPECT_TRUE( log.getKey() == key );
	const std::string newKey = "foo"; 
	EXPECT_TRUE( log.setKey( newKey ) );
	EXPECT_TRUE( log.getKey() == newKey );

	//Test the last message getter/setter
	const std::string testString = "This is a test string";
	log << testString << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString );
	const std::string testString2 = "This is another test string";
	log << testString2 << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Test the instance logging enable/disable getter/setter
	EXPECT_TRUE( log.setLoggingEnabled( true ) );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( log.getLoggingEnabled() == false );
	EXPECT_TRUE( ErrorLog::setLoggingEnabled( false ) );
	EXPECT_TRUE( ErrorLog::getLoggingEnabled() == false );
	EXPECT_TRUE( ErrorLog::setLoggingEnabled( true ) );
	EXPECT_TRUE( ErrorLog::getLoggingEnabled() == true );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( ErrorLog::getLoggingEnabled() == false );

	//Logging is off so we should NOT see this message
	log << "LOGGING IS OFF, WE SHOULD NOT SEE THIS MESSAGE" << std::endl;

	//The last message should not have changed
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Create a new log instance
	ErrorLog log2( "another-log" );

	//Logging should be disabled for log2 as well (as the logging is global across all instances)
	EXPECT_TRUE( log2.getLoggingEnabled() == false );

	//Enable logging on the first instance
	EXPECT_TRUE( log.setLoggingEnabled( true ) );

	//Logging should now be enabled across all instances
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log2.getLoggingEnabled() == true );

	//Create a new log instance and test the copy constructor
	ErrorLog log3( log2 );

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log2.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log2.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log2.getLoggingEnabled() );

	//Create a new log instance and test the equals operator
	ErrorLog log4;

	log4 = log3;

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log4.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log4.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log4.getLoggingEnabled() );
}

// Tests the TrainLog class getters/setters
TEST(Logging, TrainLogBaseClassGetterSetter) {
	const std::string key = "train-test";
	TrainingLog log( key );

	//Test the key gettter/setter
	EXPECT_TRUE( log.getKey() == key );
	const std::string newKey = "foo"; 
	EXPECT_TRUE( log.setKey( newKey ) );
	EXPECT_TRUE( log.getKey() == newKey );

	//Test the last message getter/setter
	const std::string testString = "This is a test string";
	log << testString << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString );
	const std::string testString2 = "This is another test string";
	log << testString2 << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Test the instance logging enable/disable getter/setter
	EXPECT_TRUE( log.setLoggingEnabled( true ) );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( log.getLoggingEnabled() == false );
	EXPECT_TRUE( TrainingLog::setLoggingEnabled( false ) );
	EXPECT_TRUE( TrainingLog::getLoggingEnabled() == false );
	EXPECT_TRUE( TrainingLog::setLoggingEnabled( true ) );
	EXPECT_TRUE( TrainingLog::getLoggingEnabled() == true );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( TrainingLog::getLoggingEnabled() == false );

	//Logging is off so we should NOT see this message
	log << "LOGGING IS OFF, WE SHOULD NOT SEE THIS MESSAGE" << std::endl;

	//The last message should not have changed
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Create a new log instance
	TrainingLog log2( "another-log" );

	//Logging should be disabled for log2 as well (as the logging is global across all instances)
	EXPECT_TRUE( log2.getLoggingEnabled() == false );

	//Enable logging on the first instance
	EXPECT_TRUE( log.setLoggingEnabled( true ) );

	//Logging should now be enabled across all instances
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log2.getLoggingEnabled() == true );

	//Create a new log instance and test the copy constructor
	TrainingLog log3( log2 );

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log2.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log2.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log2.getLoggingEnabled() );

	//Create a new log instance and test the equals operator
	TrainingLog log4;

	log4 = log3;

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log4.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log4.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log4.getLoggingEnabled() );
}

// Tests the TestLog class getters/setters
TEST(Logging, TestLogBaseClassGetterSetter) {
	const std::string key = "test-test";
	TestingLog log( key );

	//Test the key gettter/setter
	EXPECT_TRUE( log.getKey() == key );
	const std::string newKey = "foo"; 
	EXPECT_TRUE( log.setKey( newKey ) );
	EXPECT_TRUE( log.getKey() == newKey );

	//Test the last message getter/setter
	const std::string testString = "This is a test string";
	log << testString << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString );
	const std::string testString2 = "This is another test string";
	log << testString2 << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Test the instance logging enable/disable getter/setter
	EXPECT_TRUE( log.setLoggingEnabled( true ) );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( log.getLoggingEnabled() == false );
	EXPECT_TRUE( TestingLog::setLoggingEnabled( false ) );
	EXPECT_TRUE( TestingLog::getLoggingEnabled() == false );
	EXPECT_TRUE( TestingLog::setLoggingEnabled( true ) );
	EXPECT_TRUE( TestingLog::getLoggingEnabled() == true );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( TestingLog::getLoggingEnabled() == false );

	//Logging is off so we should NOT see this message
	log << "LOGGING IS OFF, WE SHOULD NOT SEE THIS MESSAGE" << std::endl;

	//The last message should not have changed
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Create a new log instance
	TestingLog log2( "another-log" );

	//Logging should be disabled for log2 as well (as the logging is global across all instances)
	EXPECT_TRUE( log2.getLoggingEnabled() == false );

	//Enable logging on the first instance
	EXPECT_TRUE( log.setLoggingEnabled( true ) );

	//Logging should now be enabled across all instances
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log2.getLoggingEnabled() == true );

	//Create a new log instance and test the copy constructor
	TestingLog log3( log2 );

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log2.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log2.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log2.getLoggingEnabled() );

	//Create a new log instance and test the equals operator
	TestingLog log4;

	log4 = log3;

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log4.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log4.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log4.getLoggingEnabled() );
}

// Tests the DebugLog class getters/setters
TEST(Logging, DebugLogBaseClassGetterSetter) {
	const std::string key = "debug-test";
	DebugLog log( key );

	//Test the key gettter/setter
	EXPECT_TRUE( log.getKey() == key );
	const std::string newKey = "foo"; 
	EXPECT_TRUE( log.setKey( newKey ) );
	EXPECT_TRUE( log.getKey() == newKey );

	//Test the last message getter/setter
	const std::string testString = "This is a test string";
	log << testString << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString );
	const std::string testString2 = "This is another test string";
	log << testString2 << std::endl;
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Test the instance logging enable/disable getter/setter
	EXPECT_TRUE( log.setLoggingEnabled( true ) );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( log.getLoggingEnabled() == false );
	EXPECT_TRUE( DebugLog::setLoggingEnabled( false ) );
	EXPECT_TRUE( DebugLog::getLoggingEnabled() == false );
	EXPECT_TRUE( DebugLog::setLoggingEnabled( true ) );
	EXPECT_TRUE( DebugLog::getLoggingEnabled() == true );
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log.setLoggingEnabled( false ) );
	EXPECT_TRUE( DebugLog::getLoggingEnabled() == false );

	//Logging is off so we should NOT see this message
	log << "LOGGING IS OFF, WE SHOULD NOT SEE THIS MESSAGE" << std::endl;

	//The last message should not have changed
	EXPECT_TRUE( log.getLastMessage() == testString2 );

	//Create a new log instance
	DebugLog log2( "another-log" );

	//Logging should be disabled for log2 as well (as the logging is global across all instances)
	EXPECT_TRUE( log2.getLoggingEnabled() == false );

	//Enable logging on the first instance
	EXPECT_TRUE( log.setLoggingEnabled( true ) );

	//Logging should now be enabled across all instances
	EXPECT_TRUE( log.getLoggingEnabled() == true );
	EXPECT_TRUE( log2.getLoggingEnabled() == true );

	//Create a new log instance and test the copy constructor
	DebugLog log3( log2 );

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log2.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log2.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log2.getLoggingEnabled() );

	//Create a new log instance and test the equals operator
	DebugLog log4;

	log4 = log3;

	//Test the states were copied
	EXPECT_TRUE( log3.getKey() == log4.getKey() );
	EXPECT_TRUE( log3.getLastMessage() == log4.getLastMessage() );
	EXPECT_TRUE( log3.getLoggingEnabled() == log4.getLoggingEnabled() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
