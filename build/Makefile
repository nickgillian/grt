# This is an example Makefile for building the GRT (as a static library) and also for building the GRT examples.
# This Makefile only works on Linux/OSX, Windows support will be added soon.
# To build the GRT and examples, install make (http://www.gnu.org/software/make/) and then:
# (1) Open terminal and cd to the directory containing this file
# (2) type: make
# (3) make will now build the GRT library (libgrt.a) and also build the GRT examples (which will be placed in the folder GRTExamples)
# (4) To run any of the examples, cd into the GRTExamples folder and then run: ./ExampleName
# If you just want to build the GRT as a library to use in your own projects then type: make grtlib
GRT_SRC_DIR = ../GRT
GRT_EXAMPLES_DIR = ../examples
GRT_CLASSIFICATION_MODULES = ClassificationModules/AdaBoost/*.cpp ClassificationModules/AdaBoost/WeakClassifiers/*.cpp \
                             ClassificationModules/ANBC/*.cpp \
                             ClassificationModules/BAG/*.cpp \
                             ClassificationModules/DecisionTree/*.cpp \
                             ClassificationModules/DTW/*.cpp \
                             ClassificationModules/GMM/*.cpp \
                             ClassificationModules/KNN/*.cpp \
                             ClassificationModules/LDA/*.cpp \
                             ClassificationModules/RandomForests/*.cpp \
                             ClassificationModules/Softmax/*.cpp \
                             ClassificationModules/SVM/*.cpp \
                             ClassificationModules/SVM/LIBSVM/*.cpp \
                             ClassificationModules/HMM/*.cpp \
                             ClassificationModules/MinDist/*.cpp
GRT_CLUSTERING_MODULES = ClusteringModules/GaussianMixtureModels/*.cpp \
                         ClusteringModules/KMeans/*.cpp \
                         ClusteringModules/HierarchicalClustering/*.cpp \
                         ClusteringModules/SelfOrganizingMap/*.cpp
GRT_CONTEXT_MODULES = ContextModules/*.cpp
GRT_DATA_STRUCTURES = DataStructures/*.cpp
GRT_FEATURE_EXTRACTION_MODULES = FeatureExtractionModules/FFT/*.cpp \
                                 FeatureExtractionModules/KMeansQuantizer/*.cpp \
				 FeatureExtractionModules/KMeansFeatures/*.cpp \
                                 FeatureExtractionModules/MovementIndex/*.cpp \
                                 FeatureExtractionModules/MovementTrajectoryFeatures/*.cpp \
                                 FeatureExtractionModules/SOMQuantizer/*.cpp \
                                 FeatureExtractionModules/TimeDomainFeatures/*.cpp \
                                 FeatureExtractionModules/TimeseriesBuffer/*.cpp \
                                 FeatureExtractionModules/ZeroCrossingCounter/*.cpp
GRT_CORE_ALGORITHMS = CoreAlgorithms/BernoulliRBM/*.cpp CoreAlgorithms/PrincipalComponentAnalysis/*.cpp
GRT_PIPELINE = CoreModules/*.cpp
GRT_POST_PROCESSING_MODULES = PostProcessingModules/*.cpp
GRT_PRE_PROCESSING_MODULES = PreProcessingModules/*.cpp
GRT_REGRESSION_MODULES = RegressionModules/ArtificialNeuralNetworks/MLP/*.cpp RegressionModules/LinearRegression/*.cpp RegressionModules/LogisticRegression/*.cpp RegressionModules/MultidimensionalRegression/*.cpp
GRT_UTIL = Util/*.cpp
_GRT_SRC = $(GRT_CLASSIFICATION_MODULES) $(GRT_CLUSTERING_MODULES) $(GRT_CONTEXT_MODULES) $(GRT_DATA_STRUCTURES) \
           $(GRT_FEATURE_EXTRACTION_MODULES) $(GRT_CORE_ALGORITHMS) $(GRT_PIPELINE) $(GRT_POST_PROCESSING_MODULES) \
           $(GRT_PRE_PROCESSING_MODULES) $(GRT_REGRESSION_MODULES) $(GRT_UTIL)
GRT_SRC = $(patsubst %,$(GRT_SRC_DIR)/%,$(_GRT_SRC))
GRT_OBJS = $(patsubst %.cpp,%.o,$(GRT_SRC))
GRT_HEADERS = -I$(GRT_SRC_DIR)
GRT_LIB = libgrt.a
LIB_DIR = -L.
LIBS = -lgrt
CC = g++
CFLAGS = -g -O2 -Wall $(LIB_DIR)
GRT_OBJS_DIR = objs
GRT_EXAMPLES_OUTPUT_DIR = examples

#These are the paths to the example files
#Classification Module Examples
ADABOOST_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/AdaBoostExample/AdaBoostExample.cpp
ANBC_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/ANBCExample/ANBCExample.cpp
BAG_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/BAGExample/BAGExample.cpp
DTREE_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/DecisionTreeExample/DecisionTreeExample.cpp
DTW_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/DTWExample/DTWExample.cpp
GMM_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/GMMExample/GMMExample.cpp
HMM_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/HMMExample/HMMExample.cpp
KNN_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/KNNExample/KNNExample.cpp
MINDIST_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/MinDistExample/MinDistExample.cpp
RANDOM_FORESTS_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/RandomForestsExample/RandomForestsExample.cpp
SOFTMAX_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/SoftmaxExample/SoftmaxExample.cpp
SVM_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClassificationModulesExamples/SVMExample/SVMExample.cpp
CLASSIFICATION_EXAMPLES_SRC = $(ADABOOST_EXAMPLE_SRC) $(ANBC_EXAMPLE_SRC) $(BAG_EXAMPLE_SRC) $(DTREE_EXAMPLE_SRC) $(DTW_EXAMPLE_SRC) $(GMM_EXAMPLE_SRC) $(HMM_EXAMPLE_SRC) $(KNN_EXAMPLE_SRC) $(MINDIST_EXAMPLE_SRC) $(RANDOM_FORESTS_EXAMPLE_SRC) $(SOFTMAX_EXAMPLE_SRC) $(SVM_EXAMPLE_SRC)

#Clustering Module Examples
KMEANS_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClusteringModulesExamples/KMeansExample/KMeansExample.cpp
GMM_CLUSTER_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/ClusteringModulesExamples/GaussianMixtureModelsExample/GaussianMixtureModelsExample.cpp
CLUSTERING_EXAMPLES_SRC = $(KMEANS_EXAMPLE_SRC) $(GMM_CLUSTER_EXAMPLE_SRC)

#Datastructures Examples
LCD_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/DatastructuresExamples/ClassificationDataExample/ClassificationDataExample.cpp
TSCDS_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/DatastructuresExamples/TimeSeriesClassificationDataStreamExample/TimeSeriesClassificationDataStreamExample.cpp
LRD_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/DatastructuresExamples/RegressionDataExample/RegressionDataExample.cpp
TSCD_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/DatastructuresExamples/TimeSeriesClassificationDataExample/TimeSeriesClassificationDataExample.cpp
UCD_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/DatastructuresExamples/UnlabelledDataExample/UnlabelledDataExample.cpp
DATA_STRUCTURES_EXAMPLES_SRC = $(LCD_EXAMPLE_SRC) $(TSCDS_EXAMPLE_SRC) $(LRD_EXAMPLE_SRC) $(TSCD_EXAMPLE_SRC) $(UCD_EXAMPLE_SRC)

#Feature Extraction Module Examples
FFT_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/FeatureExtractionModules/FFTExample/FFTExample.cpp
FFT_FEATURES_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/FeatureExtractionModules/FFTFeaturesExample/FFTFeaturesExample.cpp
KMEAN_QUANTIZER_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/FeatureExtractionModules/KMeansQuantizerExample/KMeansQuantizerExample.cpp
MOVEMENT_INDEX_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/FeatureExtractionModules/MovementIndexExample/MovementIndexExample.cpp
ZCF_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/FeatureExtractionModules/ZeroCrossingCounterExample/ZeroCrossingCounterExample.cpp
FEATURE_EXTRACTION_EXAMPLES_SRC = $(FFT_EXAMPLE_SRC) $(FFT_FEATURES_EXAMPLE_SRC) $(KMEAN_QUANTIZER_EXAMPLE_SRC) $(MOVEMENT_INDEX_EXAMPLE_SRC) $(ZCF_EXAMPLE_SRC)

#Post Processing Module Examples
CLASS_LABEL_CHANGE_FILTER_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/PostprocessingModulesExamples/ClassLabelChangeFilterExample/ClassLabelChangeFilterExample.cpp
CLASS_LABEL_FILTER_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/PostprocessingModulesExamples/ClassLabelFilterExample/ClassLabelFilterExample.cpp
CLASS_LABEL_TIMEOUT_FILTER_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/PostprocessingModulesExamples/ClassLabelTimeoutFilterExample/ClassLabelTimeoutFilterExample.cpp
POST_PROCESSING_EXAMPLES_SRC = $(CLASS_LABEL_CHANGE_FILTER_EXAMPLE_SRC) $(CLASS_LABEL_FILTER_EXAMPLE_SRC) $(CLASS_LABEL_TIMEOUT_FILTER_EXAMPLE_SRC) 

#Pre Processing Module Examples
DEAD_ZONE_SRC = $(GRT_EXAMPLES_DIR)/PreprocessingModulesExamples/DeadZoneExample/DeadZoneExample.cpp
DERIVATIVE_SRC = $(GRT_EXAMPLES_DIR)/PreprocessingModulesExamples/DerivativeExample/DerivativeExample.cpp
DMA_FILTER_SRC = $(GRT_EXAMPLES_DIR)/PreprocessingModulesExamples/DoubleMovingAverageFilterExample/DoubleMovingAverageFilterExample.cpp
HP_FILTER_SRC = $(GRT_EXAMPLES_DIR)/PreprocessingModulesExamples/HighPassFilterExample/HighPassFilterExample.cpp
LP_FILTER_SRC = $(GRT_EXAMPLES_DIR)/PreprocessingModulesExamples/LowPassFilterExample/LowPassFilterExample.cpp
MA_FILTER_SRC = $(GRT_EXAMPLES_DIR)/PreprocessingModulesExamples/MovingAverageFilterExample/MovingAverageFilterExample.cpp
SG_FILTER_SRC = $(GRT_EXAMPLES_DIR)/PreprocessingModulesExamples/SavitskyGolayFilterExample/SavitskyGolayFilterExample.cpp
PRE_PROCESSING_EXAMPLES_SRC = $(DEAD_ZONE_SRC) $(DERIVATIVE_SRC) $(DMA_FILTER_SRC) $(HP_FILTER_SRC) $(LP_FILTER_SRC) $(MA_FILTER_SRC) $(SG_FILTER_SRC)

#Regression Module Examples
MLP_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/RegressionModulesExamples/MLPRegressionExample/MLPRegressionExample.cpp
LINEAR_REGRESSION_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/RegressionModulesExamples/LinearRegressionExample/LinearRegressionExample.cpp
LOGISTIC_REGRESSION_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/RegressionModulesExamples/LogisticRegressionExample/LogisticRegressionExample.cpp
MULTIDIMENSIONAL_REGRESSION_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/RegressionModulesExamples/MultidimensionalRegressionExample/MultidimensionalRegressionExample.cpp
REGRESSION_EXAMPLES_SRC = $(MLP_EXAMPLE_SRC) $(LINEAR_REGRESSION_EXAMPLE_SRC) $(LOGISTIC_REGRESSION_EXAMPLE_SRC) $(MULTIDIMENSIONAL_REGRESSION_EXAMPLE_SRC)

#Tutorial Examples
HELLO_WORLD_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/Tutorials/HelloWorld/HelloWorld.cpp
MACHINE_LEARNING_101_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/Tutorials/MachineLearning101/MachineLearning101.cpp
CUSTOM_FEATURE_EXTRACTION_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/Tutorials/CustomFeatureExtraction/*.cpp
TUTORIALS_EXAMPLES_SRC = $(HELLO_WORLD_EXAMPLE_SRC) $(MACHINE_LEARNING_101_EXAMPLE_SRC) $(CUSTOM_FEATURE_EXTRACTION_EXAMPLE_SRC)

#Core Algorithms Examples
PCA_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/CoreAlgorithmsExamples/PrincipalComponentAnalysisExample/PrincipalComponentAnalysisExample.cpp
CORE_ALGORITHMS_EXAMPLES_SRC = $(PCA_EXAMPLE_SRC)

#Util Examples
SVD_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/UtilExamples/SVDExample/SVDExample.cpp
MATRIX_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/UtilExamples/MatrixExample/MatrixExample.cpp
MATRIX_DOUBLE_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/UtilExamples/MatrixDoubleExample/MatrixDoubleExample.cpp
RANDOM_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/UtilExamples/RandomExample/RandomExample.cpp
CIRCULAR_BUFFER_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/UtilExamples/CircularBufferExample/CircularBufferExample.cpp
UTIL_EXAMPLE_SRC = $(GRT_EXAMPLES_DIR)/UtilExamples/UtilExample/UtilExample.cpp
UTIL_EXAMPLES_SRC = $(SVD_EXAMPLE_SRC) $(MATRIX_EXAMPLE_SRC) $(MATRIX_DOUBLE_EXAMPLE_SRC) $(RANDOM_EXAMPLE_SRC) $(CIRCULAR_BUFFER_EXAMPLE_SRC) $(UTIL_EXAMPLE_SRC)
 
#Combine all the examples
GRT_EXAMPLES_SRC = $(CLASSIFICATION_EXAMPLES_SRC) $(CLUSTERING_EXAMPLES_SRC) $(DATA_STRUCTURES_EXAMPLES_SRC) $(FEATURE_EXTRACTION_EXAMPLES_SRC) $(POST_PROCESSING_EXAMPLES_SRC) $(PRE_PROCESSING_EXAMPLES_SRC) $(REGRESSION_EXAMPLES_SRC) $(TUTORIALS_EXAMPLES_SRC) $(CORE_ALGORITHMS_EXAMPLES_SRC) $(UTIL_EXAMPLES_SRC)

#Build the GRT static library for OSC and Linux, then the GRT examples
grt: grtlib grtExamples

#Build the GRT static library for Windows, then the GRT examples (this still needs work!)
grtWindows: grtlibWindows

#Builds the GRT as a static library for OSX and Linux
grtlib: $(GRT_SRC)
	@echo "----------------- Building GRT -----------------"
    	#Compile the GRT source code as object files
	$(CC) -c $(GRT_SRC) $(GRT_HEADERS) $(CFLAGS)
	#Move the object files to a new directoy
	mv -f `find . -maxdepth 1 -name "*.o"` $(GRT_OBJS_DIR)
	#Create the static library file
	ar rcs $(GRT_LIB) $(GRT_OBJS_DIR)/*.o

#Builds the GRT as a static library for Windows (this still needs work!)
grtlibWindows: $(GRT_SRC)
	@echo "----------------- Building GRT for Windows -----------------"
    	#Compile the GRT source code as object files
	$(CC) -c $(GRT_SRC) $(GRT_HEADERS) $(CFLAGS)
	@echo "Cleaning up object files..."
	#Move the object files to a new directory
	testMove
	#Create the static dll
	testBuild
	
testBuild:
	$(CC) -shared -o grt.dll $(GRT_OBJS_DIR)/*.o -Wl,--add-stdcall-alias
	
testMove:
	for %%i in (*.o) do @move %%i $(GRT_OBJS_DIR)

#Build the GRT examples as individual examples (linking against the GRT library)
grtExamples: $(GRT_EXAMPLES_SRC) $(GRT_SRC) $(GRT_LIB)
	@echo "----------------- Building GRT Examples -----------------"
    	#Compile the example code to create the object files
	$(CC) -c $(CLASSIFICATION_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(CLUSTERING_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(DATA_STRUCTURES_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(FEATURE_EXTRACTION_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(POST_PROCESSING_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(PRE_PROCESSING_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(REGRESSION_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(TUTORIALS_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(CORE_ALGORITHMS_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
	$(CC) -c $(UTIL_EXAMPLES_SRC) $(GRT_HEADERS) $(CFLAGS)
    	#Compile the actual output examples (which all end in .out) and link against the GRT library
	$(CC) AdaBoostExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/AdaBoostExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) ANBCExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/ANBCExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) BAGExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/BAGExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) DecisionTreeExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/DecisionTreeExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) DTWExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/DTWExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) GMMExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/GMMExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) HMMExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/HMMExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) GaussianMixtureModelsExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/GaussianMixtureModelsExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) KNNExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/KNNExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MinDistExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MinDistExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) RandomForestsExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/RandomForestsExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) SoftmaxExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/SoftmaxExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) SVMExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/SVMExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) KMeansExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/KMeansExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) ClassificationDataExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/ClassificationDataExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) TimeSeriesClassificationDataStreamExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/TimeSeriesClassificationDataStreamExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) RegressionDataExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/RegressionDataExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) TimeSeriesClassificationDataExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/TimeSeriesClassificationDataExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) UnlabelledDataExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/UnlabelledDataExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) FFTExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/FFTExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) FFTFeaturesExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/FFTFeaturesExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) KMeansQuantizerExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/KMeansQuantizerExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MovementIndexExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MovementIndexExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) PrincipalComponentAnalysisExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/PrincipalComponentAnalysisExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) ZeroCrossingCounterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/ZeroCrossingCounterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) ClassLabelChangeFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/ClassLabelChangeFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) ClassLabelFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/ClassLabelFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) ClassLabelTimeoutFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/ClassLabelTimeoutFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) DeadZoneExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/DeadZoneExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) DerivativeExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/DerivativeExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) DoubleMovingAverageFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/DoubleMovingAverageFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) HighPassFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/HighPassFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) LowPassFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/LowPassFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MovingAverageFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MovingAverageFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) SavitskyGolayFilterExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/SavitskyGolayFilterExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MLPRegressionExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MLPRegressionExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) LinearRegressionExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/LinearRegressionExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) LogisticRegressionExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/LogisticRegressionExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MultidimensionalRegressionExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MultidimensionalRegressionExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) HelloWorld.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/HelloWorld.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MachineLearning101.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MachineLearning101.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) CustomFeatureExtractionExample.o Accumulator.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/CustomFeatureExtractionExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) CircularBufferExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/CircularBufferExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MatrixDoubleExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MatrixDoubleExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) MatrixExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/MatrixExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) RandomExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/RandomExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) SVDExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/SVDExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	$(CC) UtilExample.o -o $(GRT_EXAMPLES_OUTPUT_DIR)/UtilExample.out $(GRT_HEADERS) $(CFLAGS) $(LIBS)
	
    	#Tidy things up
	rm *.o
	#Copy the example data files into the GRT_EXAMPLES_DIR
ifeq ($(shell uname),Linux)
	cp -fu `find $(GRT_EXAMPLES_DIR) -maxdepth 3 -name "*.txt"` $(GRT_EXAMPLES_OUTPUT_DIR)
	cp -fu `find $(GRT_EXAMPLES_DIR) -maxdepth 3 -name "*.csv"` $(GRT_EXAMPLES_OUTPUT_DIR)
endif
ifeq ($(shell uname),Darwin)
	cp -f `find $(GRT_EXAMPLES_DIR) -maxdepth 3 -name "*.txt"` $(GRT_EXAMPLES_OUTPUT_DIR)
	cp -f `find $(GRT_EXAMPLES_DIR) -maxdepth 3 -name "*.csv"` $(GRT_EXAMPLES_OUTPUT_DIR)
endif

cleanGRTExamples:
	rm -f $(GRT_EXAMPLES_OUTPUT_DIR)/*.out

clean: cleanGRTExamples
	rm -f *.o *.a
