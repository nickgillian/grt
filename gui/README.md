###Gesture Recognition Toolkit GUI

####Building the GUI on OSX and Linux

1. Download and install QtCreator: http://qt-project.org/
2. Open Qt and try and build and run one of the examples to ensure it is installed correctly)
3. Use the main GRT cmake file to build and install the GRT (you will find this in the build directory of the main project directory)
4. Open the main GRT qt file (GRT.pro) using QtCreator (Qt: File -> Open File or Project)
5. Select either the Debug or Release build (you might have to select 'Configure Project' the first time you open the project)
6. Check the GRT.pro file to make sure all the default include and lib directories match those on your system
7. Build the project
8. If the build is successful, press Run to launch the GUI
9. If you want to deploy the application on other machines, you should run the python script which will setup the application for deployment:

    python build-release_OSNAME.py
	
	
####Building on Windows (Visual Studio 2012 Express)

1. Download and install Qt 5.4.0 for Windows 32-bit (VS 2012, OpenGL, 643 MB) : http://www.qt.io/download-open-source/ 
2. Open Qt and try and build and run one of the examples to ensure it is installed correctly)
3. Copy the main GRT project folder to: C:\grt
4. Open the main GRT qt file (GRT.pro) using QtCreator (Qt: File -> Open File or Project)
5. Select either the Debug or Release build (you might have to select 'Configure Project' the first time you open the project)
6. Check the GRT.pro file to make sure all the default include and lib directories match those on your system
7. Build the project
8. If the build is successful, press Run to launch the GUI

Note, so far this has only been tested on Windows 7 with Qt 5.4.0 for Windows 32-bit (VS 2012, OpenGL) and boost 1_57_0 and Visual Studio 2012 Express.

#### Notes

As of GUI version 0.1.18, you no longer need to download and install boost. The GUI now uses C++11 for threading and shared pointers in replacement of boost.
