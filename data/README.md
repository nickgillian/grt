# Gesture Recognition Toolkit Datasets

This directory contains a number of datasets that can be used to test and explore the GRT.

## Accelerometer 5-Class Orientation

- **Info**: This file contains a recording of 5 tilt gestures captured from the accelerometer sensor of a WiiMote.  Each of the 5 gestures consist of a different tilt position.

- **NumberOfClasses**: 5

- **Number of dimensions**: 3

- **Number of samples in dataset**: 1526

- **Filename**: acc-orientation.grt

- **Source**: Recorded using the acceleromer in a WiMote.

- **Format**: Classification Data, formated as a GRT Classification Data file.

- **Class Labels**:
	1. tilt orientation 1
	2. tilt orientation 2
	3. tilt orientation 3
	4. tilt orientation 4
	5. tilt orientation 5

- **Features**:
	1. raw accelerometer x data
	2. raw accelerometer y data
	3. raw accelerometer z data

## Wine

- **Info**: Exact details of the dataset can be found on the UCI website http://archive.ics.uci.edu/ml/datasets/Wine, the dataset has not been modified from the version on the website.

- **Source**: The original version of this dataset can be found on the UCI machine learning repository: http://archive.ics.uci.edu/ml/datasets/Wine

- **Format**: Classification Data, formated as CSV, first column is class label (1-3)

- **Class Labels**:
	1. wine class 1
	2. wine class 2
	3. wine class 3

- **Features**:
	1. Alcohol 
	2. Malic acid 
	3. Ash 
	4. Alcalinity of ash 
	5. Magnesium 
	6. Total phenols 
	7. Flavanoids 
	8. Nonflavanoid phenols 
	9. Proanthocyanins 
	10. Color intensity 
	11. Hue 
	12. OD280/OD315 of diluted wines 
	13. Proline 

## Basic Linear Dataset

- **Info**: This dataset contains a basic linear dataset that can be used to test the GRT regression algorithms.

- **Number of dimensions**: 3

- **Number of targets**: 1

- **Number of samples in dataset**: 1000

- **Filename**: basic_linear_dataset.grt

- **Source**: Recorded using the GRT.

- **Format**: Regression Data, formated as a GRT Regression Data file.

- **Targets**:
	1. linear trend

- **Features**:
	1. target + Gaussian noise
	2. target + Gaussian noise
	3. target + Gaussian noise



