%module grt
%{
#include "GRT.h"
%}

%inline %{
// this code is copied from i file to wrap.cpp file
typedef unsigned int UINT;
using namespace GRT;
%}

%include "std_vector.i"
%include "std_string.i"

// ———— Vector ————————————
%template(VectorDoubleBase) std::vector<double>;
%include "Util/GRTTypedefs.h"

// ———— Matrix ———————————–
%include "Util/Matrix.h"
// instantiate Matrix, just to give SWIG something to do with it
// %template(MatrixDoubleBase) GRT::Matrix<double>;
// %include "Util/MatrixDouble.h"


// ——————————————————-
/* For the rest, let’s just grab the original header files here and
* let swig generate the glue code for these. Here I selected only
* those headers which are necessary for DTW, and their base classes
*/
%include "DataStructures/TimeSeriesClassificationData.h"
%include "CoreModules/Classifier.h"
%include "ClassificationModules/DTW/DTW.h"