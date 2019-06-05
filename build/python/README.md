# Python Bindings for GRT (PyGRT)

A build mechanism for creating python bindings for GRT using SWIG. 

With the GRT python module built one could write code as simply as the following:

        import GRT
        
        # Create a new Gesture Recognition Pipeline
        pipeline = GRT.GestureRecognitionPipeline()
        
        # Add a KNN classifier to the pipeline with a K value of 10
        knn = GRT.KNN(10)
        pipeline.setClassifier(knn)
    
        # Train the pipeline using the training data
        print("Training model...")
        pipeline.train(training_data)
 
 Example taken from `hello_world_example.py` provided.
 
 ## Building
 
 PyGRT will build using CMake by adding `-DBUILD_PYTHON_BINDINGS=ON` to the `cmake` execution (`ON` by default).
 
 A python "package" will be created in the `$BUILD_DIR/python` directory, 
 inside which you may run `python3` and `import GRT`.
 
 For example, an end-to-end build of GRT with PyGRT and running a python example:
 
    cd $GRT_ROOT/build/
    mkdir tmp && cd tmp
    make -j 
    cd python
    python3 hello_world_example.py ../../../data/IrisData.grt
 
 A full blown installable package (e.g. with a `setup.py`) will be created eventually, for simple deployment in `site-packages`.
 
 ## Details
 
 The file `GRT.i` provides a SWIG module that translates some key datastructures to python, as well as GRT APIs.
 1D vectors are translated to python `list`s and 2D matrices to `numpy.array`s.
 
 The PyGRT binding are not complete.
 So far we've implemented just a handful of the classifiers and datastructures.
 More will be converted in the future, however contributions are welcome. 
 Contributing is very simple. 
 
 ### Adding more functionality
 You can help extend the PyGRT API coverage by adding more `.h` files in `GRT.i`.
 
 When you add more classes, please also include an example `.py` script to test the functionality and compare it 
 to the C++ counterpart.