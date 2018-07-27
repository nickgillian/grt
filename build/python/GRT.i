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
%template(VectorTVectorFloat) GRT::Vector<GRT::VectorFloat>;

%typemap(out) GRT::Vector<UINT>, const GRT::Vector<UINT> %{
  $result = PyList_New($1.size());
  for (int i = 0; i < $1.size(); ++i) {
    PyList_SetItem($result, i, PyInt_FromLong($1[i]));
  }
%}

// python list into vec_of_ints: from Python to C++
%typemap(in) GRT::Vector< UINT >,
             const GRT::Vector< UINT >
{
    Py_ssize_t size = PyList_Size($input); //get size of the list
    for (int i = 0; i < size; i++) {
      PyObject *s = PyList_GetItem($input,i);
      if (!PyInt_Check(s)) {
        PyErr_SetString(PyExc_ValueError, "List items must be integers");
        return NULL;
      }
      $1.push_back((int)PyLong_AsLong(s)); //put the value into the array
    }
}

// Make sure vector arg is a python list
%typecheck(SWIG_TYPECHECK_POINTER) GRT::Vector< UINT >,
                                   const GRT::Vector< UINT >
%{
    $1 = PyList_Check($input);
%}

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
  $1 = PyArray_Check(arrayobj) && PyArray_ISFLOAT(arrayobj) && (PyArray_NDIM(arrayobj) == 1);
}

%typemap(out) GRT::Vector<GRT::VectorFloat>, const GRT::Vector<GRT::VectorFloat> %{
  $result = PyList_New($1.size());
  for (int i = 0; i < $1.size(); ++i) {
    npy_intp dims[1]{(npy_intp)($1[i].size())};
    PyObject* new_arr = PyArray_SimpleNew(1, dims, NPY_FLOAT);
    PyArrayObject* arr_ptr = reinterpret_cast<PyArrayObject*>(new_arr);
    for (size_t j = 0; j < $1[i].size(); ++j) {
      float* d_ptr = static_cast<float*>(PyArray_GETPTR1(arr_ptr, j));
      *d_ptr = $1[i][j];
    }
    PyList_SetItem($result, i, new_arr);
  }
%}

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
  $1 = PyArray_Check(arrayobj) && PyArray_ISFLOAT(arrayobj) && (PyArray_NDIM(arrayobj) == 2);
}

%typemap(out) GRT::Matrix< GRT::VectorFloat >,
              const GRT::Matrix< GRT::VectorFloat >
%{
  {
    npy_intp dims[3]{$1.getNumRows(),
                     $1.getNumCols(),
                     static_cast<npy_intp>($1.operator[](0)[0].size())};
    $result = PyArray_SimpleNew(3, dims, NPY_FLOAT);
    PyArrayObject* arr_ptr = reinterpret_cast<PyArrayObject*>($result);
    for (size_t i = 0; i < $1.getNumRows(); ++i) {
      for (size_t j = 0; j < $1.getNumCols(); ++j) {
        for (size_t k = 0; k < $1.operator[](i)[j].size(); ++k) {
          float* d_ptr = static_cast<float*>(PyArray_GETPTR3(arr_ptr, i, j, k));
          *d_ptr = $1.operator[](i)[j][k];
        }
      }
    }
  }
%}


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
%include "../GRT/DataStructures/ClassificationDataStream.h"
%include "../GRT/DataStructures/UnlabelledData.h"

%include "../GRT/CoreModules/MLBase.h"
%include "../GRT/CoreModules/Classifier.h"
%include "../GRT/CoreModules/PostProcessing.h"
%include "../GRT/CoreModules/Clusterer.h"
%include "../GRT/CoreModules/FeatureExtraction.h"
%include "../GRT/CoreModules/PreProcessing.h"
%include "../GRT/CoreModules/Regressifier.h"

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

%include "../GRT/ClusteringModules/ClusterTree/ClusterTreeNode.h"
%include "../GRT/ClusteringModules/ClusterTree/ClusterTree.h"
%include "../GRT/ClusteringModules/GaussianMixtureModels/GaussianMixtureModels.h"
%include "../GRT/ClusteringModules/HierarchicalClustering/HierarchicalClustering.h"
%include "../GRT/ClusteringModules/KMeans/KMeans.h"
%include "../GRT/ClusteringModules/SelfOrganizingMap/SelfOrganizingMap.h"

%include "../GRT/FeatureExtractionModules/KMeansQuantizer/KMeansQuantizer.h"
%include "../GRT/FeatureExtractionModules/FFT/FastFourierTransform.h"
%include "../GRT/FeatureExtractionModules/FFT/FFT.h"

%template(STDVectorOfFastFourierTransform) std::vector<GRT::FastFourierTransform>;
%template(GRTVectorOfFastFourierTransform) GRT::Vector<GRT::FastFourierTransform>;

%include "../GRT/FeatureExtractionModules/FFT/FFTFeatures.h"
%include "../GRT/FeatureExtractionModules/EnvelopeExtractor/EnvelopeExtractor.h"
%include "../GRT/FeatureExtractionModules/KMeansFeatures/KMeansFeatures.h"
%include "../GRT/FeatureExtractionModules/MovementIndex/MovementIndex.h"
%include "../GRT/FeatureExtractionModules/MovementTrajectoryFeatures/MovementTrajectoryFeatures.h"
%include "../GRT/FeatureExtractionModules/ZeroCrossingCounter/ZeroCrossingCounter.h"
%include "../GRT/FeatureExtractionModules/PCA/PCA.h"
%include "../GRT/FeatureExtractionModules/RBMQuantizer/RBMQuantizer.h"
%include "../GRT/FeatureExtractionModules/SOMQuantizer/SOMQuantizer.h"
%include "../GRT/FeatureExtractionModules/TimeDomainFeatures/TimeDomainFeatures.h"
%include "../GRT/FeatureExtractionModules/TimeseriesBuffer/TimeseriesBuffer.h"

%include "../GRT/PostProcessingModules/ClassLabelChangeFilter.h"
%include "../GRT/PostProcessingModules/ClassLabelFilter.h"
%include "../GRT/PostProcessingModules/ClassLabelTimeoutFilter.h"

%include "../GRT/PreProcessingModules/DeadZone.h"
%include "../GRT/PreProcessingModules/Derivative.h"
%include "../GRT/PreProcessingModules/DoubleMovingAverageFilter.h"
%include "../GRT/PreProcessingModules/FIRFilter.h"
%include "../GRT/PreProcessingModules/HighPassFilter.h"
%include "../GRT/PreProcessingModules/LeakyIntegrator.h"
%include "../GRT/PreProcessingModules/LowPassFilter.h"
%include "../GRT/PreProcessingModules/MedianFilter.h"
%include "../GRT/PreProcessingModules/MovingAverageFilter.h"
%include "../GRT/PreProcessingModules/RMSFilter.h"
%include "../GRT/PreProcessingModules/SavitzkyGolayFilter.h"
%include "../GRT/PreProcessingModules/WeightedAverageFilter.h"

%include "../GRT/DataStructures/RegressionSample.h"
%include "../GRT/DataStructures/RegressionData.h"
%extend GRT::RegressionData {
    GRT::RegressionSample& get(const int &i){
        return $self->operator[](i);
    }
}

%include "../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/Neuron.h"
%include "../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/MLP.h"
%include "../GRT/RegressionModules/LinearRegression/LinearRegression.h"
%include "../GRT/RegressionModules/LogisticRegression/LogisticRegression.h"
%include "../GRT/RegressionModules/MultidimensionalRegression/MultidimensionalRegression.h"
%include "../GRT/RegressionModules/RegressionTree/RegressionTreeNode.h"
%include "../GRT/RegressionModules/RegressionTree/RegressionTree.h"
