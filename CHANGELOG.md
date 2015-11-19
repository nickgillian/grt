#Gesture Recognition Toolkit Change Log

This file contains some notes about significant changes to the GRT.

## 19th Novemeber 2015
- **moved swig files from GRT directory to jni directory**: The .i files were causing some issues for non-swig users, so moving them to the JNI directory to clean things up, updated
.i file paths accordingly.
- **updated revision number**: 19-Nov-2015

## 2nd Novemeber 2015
- **merged dev into master**: Added new tools directory with some command line tools for training/testing various pipelines. Updated DecisionTree logging functions.

## 7th July 2015
- **merged pull request from terziman**: Fixed ParticleFilter cast issue by resolving type

## 2nd July 2015
- **merged pull request from terziman**: Added option in CMakeLists to avoid creating install targets

## 23rd June 2015
- **updated GUI version**: 0.1.18
- **new GUI features**: added timeseries mode and new timeseries data graphs.

## 18th April 2015
- **removed boost dependancy for the GRT GUI**. You no longer need to install boost to build the GRT GUI. Instead of boost, the GUI now uses C++11 for threading and shared pointers.
- **improved GRT GUI plots**. There are now additional options to control the data plots and you have more control over how fast the plots should be refreshed.

## 3rd April 2015
- **updated version**: 03-04-15
- **moved to C++11**. The toolkit has now moved to using C++11. This is to take advantage of the numerous updates in C++11, such as threading, smart pointers and lamda functions.
- **added new ThreadPool class**. The ThreadPool class can be used as a base class for managing the number of threads used by other GRT classes. It also features a useful thread pool option for batch processing large analysis tasks.
- **added instance enable/disable logging option**.  This lets you disable the logging for a specific GRT class, instead of having to disable the logging across all instances.  For example, to disable training messages for a KMeans instance called kmeans, use: 

    kmeans.setTrainingLoggingEnabled( false );

## 19th February 2015
- **merged cyberluke pull request**. This improves building the GRT on Windows, using MSVC compiler instead of mingw.
- **allowNullGestureClass defaults to true**. The default value for the allowNullGestureClass parameter for all classification data structures has been changed to true. This enables you to use the null class label value of 0 in your datasets if needed.

## 4th January 2015
- **add new Continuous HMM algorithm**. The HMM algorithm has been improved to support both Discrete and Continuous HMMs.  The toolkit used to only support Discrete HMMs (the input to the HMM had to be quantized to a discrete integer value using a quantization algorithm), but this update enables the user to now use a Continuous HMM (the input to the HMM can now consist of an N-dimensional vector like all the other GRT classifiers).  See the new HMM Discrete and HMM Continuous examples for more information on each algorithm.

## 4th December 2014 
- **updated Decision Tree, user can now dynamically select the DecisionTreeNode**. The GRT Decision Tree algorithm has been improved to enable the user to dynamically select the type of DecisionTreeNode used for the main train and prediction functions.  There are currently two types of DecisionTreeNodes, DecisionTreeThresholdNode and DecisionTreeClusterNode.  Each node will give result in a different decision boundary and one node may therefore out perform another for a given classification task.  This new update also enables the user to create their own custom DecisionTreeNode (by inheriting from the GRT::DecistionTreeNode base class), and use this in the GRT DecisionTree.

## 2nd December 2014 
- **updated GUI to version 0.1.16**. Added new graphs and fixed PCA plot bug.

## 23rd October 2014 
- **updated GUI to version 0.1.15**.

## 4th October 2014 
- **added null rejection support to Decision Tree**. The GRT Decision Tree algorithm now has full support for null rejection, like all the GRT Classifiers, you can enable this functionality in a Decision Tree instance via: 

```C++
    DecisionTree dtree;
    dtree.enableNullRejection( true );
```

  the Decision Tree algorithm will then reject the predicted class labels of unlikely predictions, setting the predicted class label to the default null rejection label of zero.

## 23rd September 2014 
- **added getSupportsNullRejection() to classifiers**. You can now query each classifier to see if it fully supports null rejection, if so then the classifier will return true when queried.

## 18th September 2014 
- **build support moved from 'make' to 'cmake'.** You can now build the toolkit using a CMakeLists file, which will autogenerate a makefile for your system.  See the readme file in the build directory for more information.
- **added abstract 'save' and 'load' functions.**  All classes that inherit from the MLBase class can now save and load their models using the more generic 'save' and 'load' functions, these can be used instead of the more complex 'saveModelToFile' and 'loadModelFromFile' functions respectively.
- **changed default fileformat extension from '.txt' to '.grt'.** The files remain an ASCII formated files (so you can still open and edit them with any text editor), but the extension is being changed to reflect the custom GRT header information in each file.

