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

#ifndef GRT_VECTOR_FLOAT_HEADER
#define GRT_VECTOR_FLOAT_HEADER

#include "..//Util/GRTTypedefs.h"
#include "Vector.h"
#include "../Util/MinMax.h"
#include "../Util/DebugLog.h"
#include "../Util/ErrorLog.h"
#include "../Util/WarningLog.h"

GRT_BEGIN_NAMESPACE

class VectorFloat : public Vector< Float >{
public:
    /**
     Default Constructor
     */
    GRT_API VectorFloat();
    
    /**
     Constructor, sets the size of the vector
     
     @param size: sets the size of the vector
     */
    GRT_API VectorFloat( const size_type size );

    /**
     Constructor, sets the size of the vector and sets all elements to value
     
     @param size: sets the size of the vector
     @param value: the value that will be written to all elements in the vector
     */
    GRT_API VectorFloat( const size_type size, const Float &value );
    
    /**
     Copy Constructor, copies the values from the rhs VectorFloat to this VectorFloat instance
     
     @param rhs: the VectorFloat from which the values will be copied
     */
    GRT_API VectorFloat(const VectorFloat &rhs);
    
    /**
     Destructor, cleans up any memory
     */
    GRT_API virtual ~VectorFloat();
    
    /**
     Defines how the data from the rhs VectorFloat should be copied to this VectorFloat
     
     @param rhs: another instance of a VectorFloat
     @return returns a reference to this instance of the VectorFloat
     */
    GRT_API VectorFloat& operator=(const VectorFloat &rhs);
    
    /**
     Defines how the data from the rhs Vector< Float > should be copied to this VectorFloat
     
     @param rhs: an instance of a Vector< Float >
     @return returns a reference to this instance of the VectorFloat
     */
    GRT_API VectorFloat& operator=(const Vector< Float > &rhs);
    
    /**
     Defines how the data from the rhs vector of VectorFloats should be copied to this VectorFloat
     
     @param rhs: a vector of VectorFloats
     @return returns a reference to this instance of the VectorFloat
     */
    GRT_API VectorFloat& operator=(const vector< VectorFloat > &rhs);
    
    /**
     Saves the vector to a CSV file.  This replaces the deprecated saveToCSVFile function.
     
     @param filename: the name of the CSV file
     @return returns true or false, indicating if the data was saved successful
     */
    GRT_API bool save(const std::string &filename) const;
    
    /**
     Loads a vector from a CSV file. This assumes that the data has been saved as rows and columns in the CSV file
     and that there are an equal number of columns per row.
     
     This replaces the deprecated loadFromCSVFile function.
     
     @param filename: the name of the CSV file
     @return returns true or false, indicating if the data was loaded successful
     */
    GRT_API bool load(const std::string &filename,const char seperator = ',');
    
    /**
     Prints the VectorFloat contents to std::cout
     
     @param title: sets the title of the data that will be printed to std::cout
     @return returns true or false, indicating if the print was successful
     */
    GRT_API bool print(const std::string title="") const;
    
    /**
     Scales the vector to a new range given by the min and max targets, this uses the minimum and maximum values in the 
     existing vector as the minSource and maxSource for min-max scaling.
     
     @return returns true if the vector was scaled, false otherwise
     */
    GRT_API bool scale(const Float minTarget,const Float maxTarget,const bool constrain = true);
    
    /**
     Scales the vector to a new range given by the min and max targets using the ranges as the source ranges.
     
     @return returns true if the vector was scaled, false otherwise
     */
    GRT_API bool scale(const Float minSource,const Float maxSource,const Float minTarget,const Float maxTarget,const bool constrain = true);

    /**
     @return returns the minimum value in the vector
     */
    GRT_API Float getMinValue() const;

    /**
     @return returns the maximum value in the vector
     */
    GRT_API Float getMaxValue() const;
    
    /**
     @return returns the mean of the vector
     */
    GRT_API Float getMean() const;
    
    /**
     @return returns the standard deviation of the vector
     */
    GRT_API Float getStdDev() const;
    
    /**
     @return returns the minimum and maximum values in the vector
     */
    GRT_API MinMax getMinMax() const;
    
protected:
  
    WarningLog warningLog;
    ErrorLog errorLog;
    
};
    
GRT_END_NAMESPACE

#endif //Header guard
