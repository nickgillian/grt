%module grt
%{
#include "../GRT/GRT.h"
%}

%inline %{
// this code is copied from i file to wrap.cpp file
typedef unsigned int UINT;
using namespace GRT;
%}

%include "std_vector.i"
%include "std_string.i"

// ———— Vector ————————————
%template(VectorFloatBase) GRT::VectorFloat;
%include "../GRT/Util/GRTTypedefs.h"
// instantiate Vector, just to give SWIG something to do with it
// %template(VectorFloatBase) GRT::VectorFloat;
// %include "../GRT/DataStructures/VectorFloat.h"

// ———— Matrix ———————————–
%include "../GRT/DataStructures/Matrix.h"
// instantiate Matrix, just to give SWIG something to do with it
// %template(MatrixFloatBase) GRT::MatrixFloat;
// %include "../GRT/DataStructures/MatrixFloat.h"

// ——————————————————-
/* For the rest, let’s just grab the original header files here and
* let swig generate the glue code for these. Here I selected only
* those headers which are necessary for DTW, and their base classes
*/
%include "../GRT/DataStructures/TimeSeriesClassificationData.h"
%include "../GRT/CoreModules/Classifier.h"
%include "../GRT/ClassificationModules/DTW/DTW.h"
