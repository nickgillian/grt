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
#include "GRTTypedefs.h"
#include "../DataStructures/VectorFloat.h"
#include "../DataStructures/MatrixFloat.h"

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
#include <dirent.h>
#endif

#ifdef __GRT_LINUX_BUILD__
//Include any Linux specific headers
#include <unistd.h>
#include <dirent.h>
#endif

GRT_BEGIN_NAMESPACE

class GRT_API Util{
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
    @return returns true if the GRT has been compiled with C++ 11 support, false otherwise 
    */
    static bool getCxx11Enabled();
    
	/**
	A cross platform function to cause a function to sleep for a set number of milliseconds.
	
	@param const unsigned int numMilliseconds: the number of milliseconds to sleep for, 1000 milliseconds = 1 second.
	@return returns true if the sleep function was called successfully, false otherwise
	*/
    static bool sleep(const unsigned int &numMilliseconds);
    
    /**
     Performs minmax scaling. The input value (x) will be scaled from the source range to the target range.
     
	 @param x: the input value to be scaled
	 @param minSource: the minimum source value (that x originates from)
	 @param maxSource: the maximum source value (that x originates from)
	 @param minTarget: the minimum target value (that x will be scaled to)
	 @param maxTarget: the maximum target value (that x will be scaled to)
     @param constrain: if true, then the value will be constrained to the minSource and maxSource
	 @return the scaled value
     */
    static Float scale(const Float &x,const Float &minSource,const Float &maxSource,const Float &minTarget,const Float &maxTarget,const bool constrain=false);

    /**
    Gets the current date and time as a string in the format: year_month_day_hour_minute_second_millisecond

    @param includeDate: if true, then the date will be included in the string, otherwise the string will just contain the time
    @return std::string: the date and time formatted as a string
    */
    static std::string timeAsString(const bool includeDate=true);

    /**
    Converts an int to a string.

    @param i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string intToString(const int &i);

    /**
    Converts an unsigned int to a string.

    @param i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string intToString(const unsigned int &i);

    /**
    Converts an unsigned int to a string.

    @param i: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(const int &i);

	/**
    Converts an unsigned int to a string.

    @param i: the value you want to convert to a string
	@return std::string: the value as a string
	*/
    static std::string toString(const unsigned int &i);

    /**
    Converts a long to a string.

    @param i: the value you want to convert to a string
    @return std::string: the value as a string
    */
    static std::string toString(const long &i);

    /**
    Converts an unsigned long to a string.

    @param i: the value you want to convert to a string
    @return std::string: the value as a string
    */
    static std::string toString(const unsigned long &i);
    
    /**
     Converts an unsigned long long to a string.
     
     @param i: the value you want to convert to a string
     @return std::string: the value as a string
     */
    static std::string toString(const unsigned long long &i);

    /**
     Converts a boolean to a string.
     
     @param b: the value you want to convert to a string
     @return std::string: the boolan as a string
     */
    static std::string toString(const bool &b);

    /**
    Converts a float to a string.

    @param v: the value you want to convert to a string
    @return std::string: the value as a string
    */
    static std::string toString(const float &v);
    
	/**
    Converts a double to a string.

    @param v: the value you want to convert to a string
    @return std::string: the value as a string
	*/
    static std::string toString(const double &v);

    /**
    Converts a long double to a string.

    @param v: the value you want to convert to a string
    @return std::string: the value as a string
    */
    static std::string toString(const long double &v);

    /**
    Converts a string to an int.

    @param s: the value you want to convert to an int
	@return int: the value as an int
	*/
    static int stringToInt(const std::string &s);

    /**
    Converts a string to a double.

    @param s: the value you want to convert to a double
	@return the value as a double
	*/
    static double stringToDouble(const std::string &s);

    /**
    Converts a string to a float.

    @param s: the value you want to convert to a float
    @return the value as a float
    */
    static Float stringToFloat(const std::string &s);
    
    /**
     Converts a string to a boolean. Any string that matches true, True, TRUE, t, T, or 1 will return true, anything else will return false.
     
     @param s: the value you want to convert to a bool
     @return bool: the value as a bool
     */
    static bool stringToBool(const std::string &s);
    
    /**
     Checks if the string (str) ends with a specific ending (such as .csv).
     
     @param str: the string you want to query
     @param ending: the ending you want to search for
     @return bool: returns true if the input string ends with the ending, false otherwise
     */
    static bool stringEndsWith(const std::string &str, const std::string &ending);
    
    /**
     Limits the input value so it is between the range of minValue and maxValue.
     If the input value is below the minValue then the output of the function will be the minValue.
     If the input value is above the maxValue then the output of the function will be the maxValue.
     Otherwise, the out of the function will be the input.
     
     @param value: the input value that should be limited
     @param minValue: the minimum value that should be limited
     @param maxValue: the maximum value that should be limited
     @return the limited double input value
     */
    static Float limit(const Float value,const Float minValue,const Float maxValue);

    /**
    Computes the sum of the vector x.

    @param x: the vector of values you want to sum
	@return double: the sum of the input vector x
	*/
    static Float sum(const VectorFloat &x);

    /**
    Computes the dot product between the two input vectors. The two input vectors must have the same size.

    @param a: the first vector for the dot product
    @param b: the second vector for the dot product
	@return double: the dot product between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static Float dotProduct(const VectorFloat &a,const VectorFloat &b);

    /**
    Computes the euclidean distance between the two input vectors. The two input vectors must have the same size.

    @param a: the first vector for the euclidean distance
    @param b: the second vector for the euclidean distance
	@return the euclidean distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static Float euclideanDistance(const VectorFloat &a,const VectorFloat &b);

    /**
    Computes the squared euclidean distance between the two input vectors. The two input vectors must have the same size.

    @param a: the first vector for the euclidean distance
    @param b: the second vector for the euclidean distance
    @return the euclidean distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
    */
    static Float squaredEuclideanDistance(const VectorFloat &a,const VectorFloat &b);

    /**
    Computes the manhattan distance between the two input vectors. The two input vectors must have the same size.
    The manhattan distance is also known as the L1 norm, taxicab distance, city block distance, or rectilinear distance.

    @param a: the first vector for the manhattan distance
    @param b: the second vector for the manhattan distance
	@return the manhattan distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static Float manhattanDistance(const VectorFloat &a,const VectorFloat &b);

    /**
    Computes the cosine distance between the two input vectors. The two input vectors must have the same size.
    The cosine distance can be used as a similarity measure, the distance ranges from −1 meaning exactly opposite, to 1 meaning exactly the same, 
    with 0 usually indicating independence, and in-between values indicating intermediate similarity or dissimilarity.

    @param a: the first vector for the cosine distance
    @param b: the second vector for the cosine distance
	@return the cosine distance between the two input vectors, if the two input vectors are not the same size then the dist will be INF
	*/
    static Float cosineDistance(const VectorFloat &a,const VectorFloat &b);
    
    /**
     Scales the vector from a source range to the new target range
     
     @param x: the input value to be scaled
	 @param minSource: the minimum source value (that x originates from)
	 @param maxSource: the maximum source value (that x originates from)
	 @param minTarget: the minimum target value (that x will be scaled to)
	 @param maxTarget: the maximum target value (that x will be scaled to)
     @param constrain: if true, then the value will be constrained to the minSource and maxSource
     @return the scaled input vector
     */
	static VectorFloat scale(const VectorFloat &x,const Float minSource,const Float maxSource,const Float minTarget=0,const Float maxTarget=1,const bool constrain=false);
    
    /**
    Normalizes the input vector x so the sum is 1.

    @param x: the vector of values you want to normalize
	@return the normalized input vector (the sum of which should be 1)
	*/
	static VectorFloat normalize(const VectorFloat &x);
    
    /**
     Limits the input data x so each element is within the range [minValue maxValue]. 
     Returns a new vector with the limited data.
     
     @param x: the vector of values you want to limit
     @param minValue: the minimum value
     @param maxValue: the maximum value
     @return the limited input vector
     */
	static VectorFloat limit(const VectorFloat &x,const Float minValue,const Float maxValue);
	
	/**
    Gets the minimum value in the input vector.

    @param x: the vector of values you want to find the minimum value for
	@return the minimum value in the input vector, this will be INF if the input vector size is 0
	*/
    static Float getMin(const VectorFloat &x);
    
    /**
     Gets the index of the minimum value in the input vector.
     
     @param x: the vector of values you want to find the minimum index value for
     @return the index of the minimum value in the vector
     */
    static unsigned int getMinIndex(const VectorFloat &x);

    /**
    Gets the maximum value in the input vector.

    @param x: the vector of values you want to find the maximum value for
	@return the maximum value in the input vector, this will be INF if the input vector size is 0
	*/
    static Float getMax(const VectorFloat &x);
    
    /**
     Gets the index of the maximum value in the input vector.
     
     @param x: the vector of values you want to find the maximum index value for
     @return the index of the maximum value in the vector
     */
    static unsigned int getMaxIndex(const VectorFloat &x);

    /**
    Gets the minimum value in the input vector.

    @param x: the vector of values you want to find the minimum value for
	@return the minimum value in the input vector, this will be INF if the input vector size is 0
	*/
    static unsigned int getMin(const std::vector< unsigned int > &x);

    /**
    Gets the maximum value in the input vector.

    @param x: the vector of values you want to find the maximum value for
	@return the maximum value in the input vector, this will be INF if the input vector size is 0
	*/
    static unsigned int getMax(const std::vector< unsigned int > &x);

    /**
    Gets the current operating system as a Util::OperatingSystems enum.

	@return unsigned int: the current operating system (which will be one of the Util::OperatingSystems enums)
	*/
	static unsigned int getOS();
    
    /**
     Converts the cartesian values {x y} into polar values {r theta}
     
     @param x: the x cartesian value
     @param y: the y cartesian value
     @param r: the return radius value
     @param theta: the return theta value
     @return void
     */
    static void cartToPolar(const Float x,const Float y,Float &r, Float &theta);
    
    /**
     Converts the polar values {r theta} into the cartesian values {x y}.
     
     @param r: the radius polar value
     @param theta: the theta polar value
     @param x: the return x value
     @param y: the return y value
     @return void
     */
    static void polarToCart(const Float r,const Float theta,Float &x, Float &y);

    /**
     @brief Parses a directory and returns a list of filenames in that directory that match the file type.
     A wildcard (.*) can be used to indicate any file in the directory.  Multiple filetypes can be searched for by seperating
     the file type via |. For example: *.csv|*.grt will return any CSV or GRT files in the directory.

     @note only supported on OSX or Linux systems at this time.

     @param directoryPath: the path of the directory you want to search (can be relative or absolute)
     @param type: sets the file type that should be searched for (e.g. .csv)
     @param filenames: will return a list of filenames found in the directory
     @return returns true if the directory was parsed, false otherwise
     */
    static bool parseDirectory( const std::string directoryPath, const std::string type, std::vector< std::string > &filenames );
    
    /**
    A list of operating systems.
    */
	enum OperatingSystems{OS_UNKNOWN=0,OS_OSX,OS_LINUX,OS_WINDOWS};

};
    
GRT_END_NAMESPACE

#endif // GRT_UTIL_HEADER
