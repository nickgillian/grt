####Gesture Recognition Toolkit (GRT) GUI Instructions

Tips for building the GUI:

[1] Download and install QTCreator: http://qt-project.org/
[2] Download, install, and compile boost: http://www.boost.org/users/download/ (the GUI assumes boost will be installed in /usr/local/include and /usr/local/lib on OSX and Linux machines)
[3] Use the main GRT cmake file to build and install the GRT (you will find this in the build directory of the main project directory)
[4] Open the main qt file (GRT.pro) using QTCreator
[5] Select either the Debug or Release build
[6] Check the GRT.pro file to make sure all the default include and lib directories match those on your system
[7] Build the project, you should now be able to run the GRT GUI application
[8] If you want to deploy the application on other machines, you should run the python script which will setup the application for deployment:

    python build-release_OSNAME.py

NOTE: So far, the GRT GUI has only been tested on OS X and Linux (Ubuntu 14.04), Windows and other Linux versions will be added soon.