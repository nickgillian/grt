/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file contains the Util class, a wrapper for a number of generic functions that are used throughout the GRT.
 This includes functions for scaling data, finding the minimum or maximum values in a double or UINT vector, etc.
 Many of these functions are static functions, which enables you to use them without having to create a new Util instance, 
 for instance, you can directly call: Util::sleep( 1000 ); to use the sleep function. 
 */

#ifndef GRT_UTIL_HEADER
#define GRT_UTIL_HEADER

//Include the GRTVersionInfo header to find which operating system we are building for
#include "GRTVersionInfo.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <cmath>

#ifdef __GRT_WINDOWS_BUILD__
//Include any Windows specific headers
#include <windows.h>
//Hey User: Make sure you add the path to the Kernel32.lib to your lib search paths
//#pragma comment(lib,"Kernel32.lib")
//The min and max macros cause major headaches, so undefine them
#undef min
#undef max
#endif

#ifdef __GRT_OSX_BUILD__
//Include any OSX specific headers
#include <unistd.h>
#endif

#ifdef __GRT_LINUX_BUILD__
//Include any Linux specific headers
#include <unistd.h>
#endif

namespace GRT{

class Util{
public:
	/**
	Default constructor.
	*/
    Util(){}

	/**
	Default destructor.
	*/
    ~Util(){}
    
	/**
	A cross platform function to cause a function to sleep for a set number of milliseconds.
	
	@param const unsigned int numMilliseconds: the number of milliseconds to sleep for, 1000 milliseconds = 1 second.
	@return returns true if the sleep function was called successfully, false otherwise
	*/
    static bool sleep(const unsigned int &numMilliseconds);
    
    /**
     Performs minmax scaling. The input value (x) will be scaled from the source range to the target range.
     
	 @param double x: the input value to be scaled
	 @param const double minSource: the minimum source value (that x originates from)
	 @param const double maxSource: the maximum source value (that x originates from)
	 @param const double minTarget: the minimum target value (that x will be scaled to)
	 @param const double maxTarget: the maximum target value (that x will be scaled to)
     @param const bool constrain: if true, then the value will be constrained to the minSource and maxSource
	 @return the scaled value
     */
    static double scale(const double &x,const double &minSource,const double &maxSource,const double &minTarget,const double &maxTarget,const bool constrain=false);

    /**
    Converts an int to a string.

    @param const int i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string intToString(const int &i);

    /**
    Converts an unsigned int to a string.

    @param const unsigned int i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string intToString(const unsigned int &i);

    /**
    Converts an unsigned int to a string.

    @param const unsigned int i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(const int &i);

	/**
    Converts an unsigned int to a string.

    @param const unsigned int i: the value you want to convert to a string
	@return std::string: the value as a string
	*/
    static std::string toString(const unsigned int &i);

    /**
    Converts a long to a string.

    @param const long i: the value you want to convert to a string
    @return std::string: the value as a string
    */
    static std::string toString(const long &i);

    /**
    Converts an unsigned long to a string.

    @param const unsigned long i: the value you want to convert to a string
    @return std::string: the value as a string
    */
    static std::string toString(const unsigned long &i);
    
    /**
     Converts an unsigned long long to a string.
     
     @param const unsigned long long i: the value you want to convert to a string
     @return std::string: the value as a string
     */
    static std::string toString(const unsigned long long &i);

    /**
     Converts a boolean to a string.
     
     @param const bool b: the value you want to convert to a string
     @return std::string: the boolan as a string
     */
    static std::string toString(const bool &b);
    
	/**
    Converts a double to a string.

    @param const double v: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(const double &v);

    /**
    Converts a long double to a string.

    @param const long double v: the value you want to convert to a string
    @return std::string: the value as a string
    */
    static std::string toString(const long double &v);

	/**
    Converts a float to a string.

    @param const float v: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(const float &v);

    /**
    Converts a string to an int.

    @param const std::string &s: the value you want to convert to an int
	@return int: the value as an int
	*/
    static int stringToInt(const std::string &s);

    /**
    Converts a string to a double.

    @param const std::string &s: the value you want to convert to a double
	@return double: the value as a double
	*/
    static double stringToDouble(const std::string &s);
    
    /**
     Converts a string to a boolean. Any string that matches true, True, TRUE, t, T, or 1 will return true, anything else will return false.
     
     @param const std::string &s: the value you want to convert to a bool
     @return bool: the value as a bool
     */
    static bool stringToBool(const std::string &s);
    
    /**
     Limits the input value so it is between the range of minValue and maxValue.
     If the input value is below the minValue then the output of the function will be the minValue.
     If the input value is above the maxValue then the output of the function will be the maxValue.
     Otherwise, the out of the function will be the input.
     
     @param const double value: the input value that should be limited
     @param const double minValue: the minimum value that should be limited
     @param const double maxValue: the maximum value that should be limited
     @return the limited double input value
     */
    static double limit(const double value,const double minValue,const double maxValue);

    /**
    Computes the sum of the vector x.

    @param const std::vector<double> &x: the vector of values you want to sum
	@return double: the sum of the input vector x
	*/
    static double sum(const std::vector<double> &x);

    /**
    Computes the dot product between the two input vectors. The two input vectors must have the same size.

    @param const std::vector<double> &a: the first vector for the dot product
    @param const std::vector<double> &a: the second vector for the dot product
	@return double: the dot product between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double dotProduct(const std::vector<double> &a,const std::vector<double> &b);

    /**
    Computes the euclidean distance between the two input vectors. The two input vectors must have the same size.

    @param const std::vector<double> &a: the first vector for the euclidean distance
    @param const std::vector<double> &a: the second vector for the euclidean distance
	@return double: the euclidean distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double euclideanDistance(const std::vector<double> &a,const std::vector<double> &b);

    /**
    Computes the manhattan distance between the two input vectors. The two input vectors must have the same size.
    The manhattan distance is also known as the L1 norm, taxicab distance, city block distance, or rectilinear distance.

    @param const std::vector<double> &a: the first vector for the manhattan distance
    @param const std::vector<double> &a: the second vector for the manhattan distance
	@return double: the manhattan distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double manhattanDistance(const std::vector<double> &a,const std::vector<double> &b);

    /**
    Computes the cosine distance between the two input vectors. The two input vectors must have the same size.
    The cosine distance can be used as a similarity measure, the distance ranges from −1 meaning exactly opposite, to 1 meaning exactly the same, 
    with 0 usually indicating independence, and in-between values indicating intermediate similarity or dissimilarity.

    @param const std::vector<double> &a: the first vector for the cosine distance
    @param const std::vector<double> &a: the second vector for the cosine distance
	@return double: the cosine distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static double cosineDistance(const std::vector<double> &a,const std::vector<double> &b);
    
    /**
     Scales the vector from a source range to the new target range
     
     @param double x: the input value to be scaled
	 @param const double minSource: the minimum source value (that x originates from)
	 @param const double maxSource: the maximum source value (that x originates from)
	 @param const double minTarget: the minimum target value (that x will be scaled to)
	 @param const double maxTarget: the maximum target value (that x will be scaled to)
     @param const bool constrain: if true, then the value will be constrained to the minSource and maxSource
     @return std::vector<double>: the scaled input vector
     */
	static std::vector<double> scale(const std::vector<double> &x,const double minSource,const double maxSource,const double minTarget=0,const double maxTarget=1,const bool constrain=false);
    
    /**
    Normalizes the input vector x so the sum is 1.

    @param const std::vector<double> &x: the vector of values you want to normalize
	@return std::vector<double>: the normalized input vector (the sum of which will be 1)
	*/
	static std::vector<double> normalize(const std::vector<double> &x);
    
    /**
     Limits the input data x so each element is within the range [minValue maxValue]. 
     Returns a new vector with the limited data.
     
     @param const std::vector<double> &x: the vector of values you want to limit
     @param const minValue: the minimum value
     @param const maxValue: the maximum value
     @return std::vector<double>: the limited input vector
     */
	static std::vector<double> limit(const std::vector<double> &x,const double minValue,const double maxValue);
	
	/**
    Gets the minimum value in the input vector.

    @param const std::vector<double> &x: the vector of values you want to find the minimum value for
	@return double: the minimum value in the input vector, this will be INF if the input vector size is 0
	*/
    static double getMin(const std::vector< double > &x);

    /**
    Gets the maximum value in the input vector.

    @param const std::vector<double> &x: the vector of values you want to find the maximum value for
	@return double: the maximum value in the input vector, this will be INF if the input vector size is 0
	*/
    static double getMax(const std::vector< double > &x);

    /**
    Gets the minimum value in the input vector.

    @param const std::vector<unsigned int> &x: the vector of values you want to find the minimum value for
	@return unsigned int: the minimum value in the input vector, this will be INF if the input vector size is 0
	*/
    static unsigned int getMin(const std::vector< unsigned int > &x);

    /**
    Gets the maximum value in the input vector.

    @param const std::vector<unsigned int> &x: the vector of values you want to find the maximum value for
	@return unsigned int: the maximum value in the input vector, this will be INF if the input vector size is 0
	*/
    static unsigned int getMax(const std::vector< unsigned int > &x);

    /**
    Gets the current operating system as a Util::OperatingSystems enum.

	@return unsigned int: the current operating system (which will be one of the Util::OperatingSystems enums)
	*/
	static unsigned int getOS();
    
    /**
     Converts the cartesian values {x y} into polar values {r theta}
     
     @param const double x: the x cartesian value
     @param const double y: the y cartesian value
     @param double &r: the return radius value
     @param double &theta: the return theta value
     @return void
     */
    static void cartToPolar(const double x,const double y,double &r, double &theta);
    
    /**
     Converts the polar values {r theta} into the cartesian values {x y}.
     
     @param const double r: the radius polar value
     @param const double theta: the theta polar value
     @param double &x: the return x value
     @param double &y: the return y value
     @return void
     */
    static void polarToCart(const double r,const double theta,double &x, double &y);
    
    /**
    A list of operating systems.
    */
	enum OperatingSystems{OS_UNKNOWN=0,OS_OSX,OS_LINUX,OS_WINDOWS};

};
    
} //End of namespace GRT

#endif // GRT_UTIL_HEADER
