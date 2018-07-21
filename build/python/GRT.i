%module(naturalvar=1) GRT

%init {
    import_array();
}

%{
/* Includes the header in the wrapper code */
#include "../GRT/GRT.h"
#include "../GRT/Util/GRTTypedefs.h"
#include "../GRT/DataStructures/Vector.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/npy_common.h"
#include "numpy/ndarrayobject.h"
#include "numpy/ndarraytypes.h"
#include "numpy/arrayobject.h"

#include <stdio.h>

using namespace GRT;
%}


%include "std_vector.i"
namespace std {
   %template(IntVector) vector<int>;
   %template(DoubleVector) vector<double>;
   %template(FloatVector) vector<float>;
}

%include "std_string.i"
using std::string;

//%include "typemaps.i"

%apply int { UINT };
%apply int { GRT::UINT };
%apply float { Float };
%apply float { GRT::Float };

/* Parse the header file to generate wrappers */
%include "../GRT/Util/GRTTypedefs.h"
%include "../GRT/Util/GRTVersionInfo.h"
%include "../GRT/Util/GRTCommon.h"
%include "../GRT/Util/Util.h"
%include "../GRT/CoreModules/GRTBase.h"

%template(UINTVector) std::vector<UINT>;
%include "../GRT/DataStructures/Vector.h"
%template(VectorTUINT) GRT::Vector<UINT>;
%template(VectorTFloat) GRT::Vector<Float>;

%typemap(out) GRT::Vector<UINT> %{
  $result = PyList_New($1.size());
  for (int i = 0; i < $1.size(); ++i) {
    PyList_SetItem($result, i, PyInt_FromLong($1[i]));
  }
%}

// python list into vec_of_ints: from Python to C++
%typemap(in) GRT::Vector< UINT >
{
    int i;
    if (!PyList_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expecting a list");
      return NULL;
    }
    Py_ssize_t size = PyList_Size($input); //get size of the list
    for (i = 0; i < size; i++)
    {
      PyObject *s = PyList_GetItem($input,i);
      if (!PyInt_Check(s))
        {
         PyErr_SetString(PyExc_ValueError, "List items must be integers");
         return NULL;
        }
      $1.push_back((int)PyInt_AS_LONG(s)); //put the value into the array
    }
}

%include "../GRT/DataStructures/VectorFloat.h"
// From GRT::VectorFloat to numpy.array
%typemap(out) GRT::Vector<Float>,
              GRT::VectorFloat,
              const GRT::Vector<Float>,
              const GRT::VectorFloat%{
  {
    npy_intp dims[1]{(npy_intp)($1.size())};
    $result = PyArray_SimpleNew(1, dims, NPY_FLOAT);
    PyArrayObject* arr_ptr = reinterpret_cast<PyArrayObject*>($result);
    for (size_t i = 0; i < $1.size(); ++i) {
      float* d_ptr = static_cast<float*>(PyArray_GETPTR1(arr_ptr, i));
      *d_ptr = $1[i];
    }
  }
%}

%typemap(out) GRT::VectorFloat&,
              GRT::VectorFloat*,
              const GRT::VectorFloat&,
              const GRT::VectorFloat* %{
  {
    npy_intp dims[1]{(npy_intp)($1->size())};
    $result = PyArray_SimpleNew(1, dims, NPY_FLOAT);
    PyArrayObject* arr_ptr = reinterpret_cast<PyArrayObject*>($result);
    for (size_t i = 0; i < $1->size(); ++i) {
      float* d_ptr = static_cast<float*>(PyArray_GETPTR1(arr_ptr, i));
      *d_ptr = $1->operator[](i);
    }
  }
%}



%typemap(in) GRT::Vector<Float>,
             const GRT::Vector<Float>,
             GRT::VectorFloat,
             const GRT::VectorFloat %{
  {
    PyArrayObject* arrayobj = reinterpret_cast<PyArrayObject*>($input);
    npy_intp size = PyArray_SIZE(arrayobj); //get size of the 1d array

    $1 = VectorFloat();

    for (npy_intp i = 0; i < size; i++) {
      void* itemptr = PyArray_GETPTR1(arrayobj, i);
      PyObject *s = PyArray_GETITEM(arrayobj, reinterpret_cast<char *>(itemptr));
      if (!PyFloat_Check(s)) {
        PyErr_SetString(PyExc_ValueError, "List items must be floats");
        return NULL;
      }
      $1.push_back((float)PyFloat_AsDouble(s)); //put the value into the array
    }
  }
%}


%typemap(in)
    GRT::Vector<Float>&,
    const GRT::Vector<Float>&,
    GRT::VectorFloat&,
    const GRT::VectorFloat&,
    GRT::VectorFloat const& %{
  {
    PyArrayObject* arrayobj = reinterpret_cast<PyArrayObject*>($input);
    npy_intp size = PyArray_SIZE(arrayobj); //get size of the 1d array

    $1 = new VectorFloat();

    for (npy_intp i = 0; i < size; i++) {
      void* itemptr = PyArray_GETPTR1(arrayobj, i);
      PyObject *s = PyArray_GETITEM(arrayobj, reinterpret_cast<char *>(itemptr));
      if (!PyFloat_Check(s)) {
        PyErr_SetString(PyExc_ValueError, "List items must be floats");
        return NULL;
      }
      $1->push_back((float)PyFloat_AsDouble(s)); //put the value into the array
    }
  }
%}

%typemap(freearg) GRT::VectorFloat&, const GRT::VectorFloat&, GRT::VectorFloat const& {
  if ($1 != 0) {
    delete($1);
  }
}

%typecheck(SWIG_TYPECHECK_POINTER)
    GRT::Vector<Float>,
    GRT::VectorFloat,
    const GRT::VectorFloat,
    GRT::VectorFloat&,
    const GRT::VectorFloat&,
    GRT::VectorFloat const&
{
  PyArrayObject* arrayobj = reinterpret_cast<PyArrayObject*>($input);
  $1 = PyArray_ISFLOAT(arrayobj) && (PyArray_NDIM(arrayobj) == 1);
}


%include "../GRT/DataStructures/Matrix.h"
%template(MatrixTFloat) GRT::Matrix<Float>;
%include "../GRT/DataStructures/MatrixFloat.h"

// From GRT::MatrixFloat to numpy.array
%typemap(out) GRT::Matrix<Float>,
              const GRT::Matrix<Float>,
              GRT::MatrixFloat,
              const GRT::MatrixFloat %{
  {
    npy_intp dims[2]{$1.getNumRows(), $1.getNumCols()};
    $result = PyArray_SimpleNew(2, dims, NPY_FLOAT);
    PyArrayObject* arr_ptr = reinterpret_cast<PyArrayObject*>($result);
    for (size_t i = 0; i < $1.getNumRows(); ++i) {
      for (size_t j = 0; j < $1.getNumCols(); ++j) {
        float* d_ptr = static_cast<float*>(PyArray_GETPTR2(arr_ptr, i, j));
        *d_ptr = $1[i][j];
      }
    }
  }
%}

%typemap(out) GRT::Matrix<Float>&,
              const GRT::Matrix<Float>&,
              GRT::MatrixFloat&,
              const GRT::MatrixFloat&,
              GRT::MatrixFloat*,
              const GRT::MatrixFloat* %{
  {
    npy_intp dims[2]{$1->getNumRows(), $1->getNumCols()};
    $result = PyArray_SimpleNew(2, dims, NPY_FLOAT);
    PyArrayObject* arr_ptr = reinterpret_cast<PyArrayObject*>($result);
    for (size_t i = 0; i < $1->getNumRows(); ++i) {
      for (size_t j = 0; j < $1->getNumCols(); ++j) {
        float* d_ptr = static_cast<float*>(PyArray_GETPTR2(arr_ptr, i, j));
        *d_ptr = $1->operator[](i)[j];
      }
    }
  }
%}

%typemap(in)
    GRT::Matrix<Float>&,
    const GRT::Matrix<Float>&,
    GRT::MatrixFloat&,
    const GRT::MatrixFloat&,
    GRT::MatrixFloat const&,
    GRT::MatrixFloat*,
    const GRT::MatrixFloat* %{
  {
    PyArrayObject* arrayobj = reinterpret_cast<PyArrayObject*>($input);
    npy_intp nrows = PyArray_DIM(arrayobj, 0);
    npy_intp ncols = PyArray_DIM(arrayobj, 1);

    $1 = new MatrixFloat(nrows, ncols);

    for (npy_intp i = 0; i < nrows; i++) {
      for (npy_intp j = 0; j < ncols; j++) {
        void* itemptr = PyArray_GETPTR2(arrayobj, i, j);
        PyObject *s = PyArray_GETITEM(arrayobj, reinterpret_cast<char *>(itemptr));
        if (!PyFloat_Check(s)) {
          PyErr_SetString(PyExc_ValueError, "Array items must be floats");
          return NULL;
        }
        $1->operator[](i)[j] = (float)PyFloat_AsDouble(s); //put the value into the array
      }
    }
  }
%}

%typemap(in)
    GRT::Matrix<Float>,
    const GRT::Matrix<Float>,
    GRT::MatrixFloat,
    const GRT::MatrixFloat,
    GRT::MatrixFloat const %{
  {
    PyArrayObject* arrayobj = reinterpret_cast<PyArrayObject*>($input);
    npy_intp nrows = PyArray_DIM(arrayobj, 0);
    npy_intp ncols = PyArray_DIM(arrayobj, 1);

    $1 = MatrixFloat(nrows, ncols);

    for (npy_intp i = 0; i < nrows; i++) {
      for (npy_intp j = 0; j < ncols; j++) {
        void* itemptr = PyArray_GETPTR2(arrayobj, i, j);
        PyObject *s = PyArray_GETITEM(arrayobj, reinterpret_cast<char *>(itemptr));
        if (!PyFloat_Check(s)) {
          PyErr_SetString(PyExc_ValueError, "Array items must be floats");
          return NULL;
        }
        $1[i][j] = (float)PyFloat_AsDouble(s); //put the value into the array
      }
    }
  }
%}

%typemap(freearg) GRT::Matrix<Float>&,
                  const GRT::Matrix<Float>&,
                  GRT::MatrixFloat&,
                  const GRT::MatrixFloat&,
                  GRT::MatrixFloat const& {
  if ($1 != 0) {
    delete($1);
  }
}

%typecheck(SWIG_TYPECHECK_POINTER)
    GRT::Matrix<Float>,
    GRT::MatrixFloat,
    const GRT::MatrixFloat,
    GRT::MatrixFloat&,
    const GRT::MatrixFloat&,
    GRT::MatrixFloat const&
{
  PyArrayObject* arrayobj = reinterpret_cast<PyArrayObject*>($input);
  $1 = PyArray_ISFLOAT(arrayobj) && (PyArray_NDIM(arrayobj) == 2);
}


%include "../GRT/DataStructures/ClassificationData.h"
%extend GRT::ClassificationData {
    GRT::ClassificationSample& get(const int &i){
        return $self->operator[](i);
    }
}

%include "../GRT/DataStructures/ClassificationSample.h"

%include "../GRT/DataStructures/TimeSeriesClassificationData.h"
%extend GRT::TimeSeriesClassificationData {
    GRT::TimeSeriesClassificationSample& get(const int &i){
        return $self->operator[](i);
    }
}

%include "../GRT/DataStructures/TimeSeriesClassificationSample.h"

%include "../GRT/CoreModules/MLBase.h"
%include "../GRT/CoreModules/Classifier.h"
%include "../GRT/CoreModules/GestureRecognitionPipeline.h"
%include "../GRT/ClassificationModules/KNN/KNN.h"
%include "../GRT/ClassificationModules/AdaBoost/WeakClassifiers/WeakClassifier.h"
%include "../GRT/ClassificationModules/AdaBoost/WeakClassifiers/DecisionStump.h"
%include "../GRT/ClassificationModules/AdaBoost/WeakClassifiers/RadialBasisFunction.h"
%include "../GRT/ClassificationModules/AdaBoost/AdaBoost.h"
%include "../GRT/ClassificationModules/ANBC/ANBC_Model.h"
%include "../GRT/ClassificationModules/ANBC/ANBC.h"
%include "../GRT/ClassificationModules/BAG/BAG.h"
%include "../GRT/CoreAlgorithms/Tree/Node.h"
%include "../GRT/CoreAlgorithms/Tree/Tree.h"
%include "../GRT/ClassificationModules/DecisionTree/DecisionTreeNode.h"
%include "../GRT/ClassificationModules/DecisionTree/DecisionTreeThresholdNode.h"
%include "../GRT/ClassificationModules/DecisionTree/DecisionTreeTripleFeatureNode.h"
%include "../GRT/ClassificationModules/DecisionTree/DecisionTreeClusterNode.h"
%include "../GRT/ClassificationModules/DecisionTree/DecisionTree.h"
%include "../GRT/ClassificationModules/DTW/DTW.h"
%include "../GRT/CoreAlgorithms/ParticleFilter/Particle.h"
%include "../GRT/CoreAlgorithms/ParticleFilter/ParticleFilter.h"
%include "../GRT/ClassificationModules/FiniteStateMachine/FSMParticle.h"
%include "../GRT/ClassificationModules/FiniteStateMachine/FiniteStateMachine.h"
%include "../GRT/ClassificationModules/GMM/MixtureModel.h"
%include "../GRT/ClassificationModules/GMM/GMM.h"
%include "../GRT/ClassificationModules/HMM/HMMEnums.h"
%include "../GRT/ClassificationModules/HMM/ContinuousHiddenMarkovModel.h"
%include "../GRT/ClassificationModules/HMM/DiscreteHiddenMarkovModel.h"
%include "../GRT/ClassificationModules/HMM/HMM.h"
%include "../GRT/ClassificationModules/KNN/KNN.h"
%include "../GRT/ClassificationModules/MinDist/MinDistModel.h"
%include "../GRT/ClassificationModules/MinDist/MinDist.h"
%include "../GRT/ClassificationModules/ParticleClassifier/ParticleClassifierParticleFilter.h"
%include "../GRT/ClassificationModules/ParticleClassifier/ParticleClassifier.h"
%include "../GRT/ClassificationModules/RandomForests/RandomForests.h"
%include "../GRT/ClassificationModules/Softmax/SoftmaxModel.h"
%include "../GRT/ClassificationModules/Softmax/Softmax.h"
%include "../GRT/ClassificationModules/SVM/SVM.h"
%include "../GRT/ClassificationModules/SwipeDetector/SwipeDetector.h"

%include "../GRT/CoreModules/Clusterer.h"
%include "../GRT/ClusteringModules/ClusterTree/ClusterTreeNode.h"
%include "../GRT/ClusteringModules/ClusterTree/ClusterTree.h"
%include "../GRT/ClusteringModules/GaussianMixtureModels/GaussianMixtureModels.h"
%include "../GRT/ClusteringModules/HierarchicalClustering/HierarchicalClustering.h"
%include "../GRT/ClusteringModules/KMeans/KMeans.h"
%include "../GRT/ClusteringModules/SelfOrganizingMap/SelfOrganizingMap.h"

%include "../GRT/CoreModules/FeatureExtraction.h"
%include "../GRT/FeatureExtractionModules/KMeansQuantizer/KMeansQuantizer.h"