----------------------------------------------------
 Gesture Recognition Toolkit (GRT) GUI Instructions
----------------------------------------------------

Tips for building the GUI:

[1] Download and install QTCreator: http://qt-project.org/
[2] Download, install, and compile boost: http://www.boost.org/users/download/
[3] Copy the following boost static libraries into this folder: gesture-recognition-toolkit/GRTGUI/GRT/Resources/OSX/Libs
- libboost_chrono.a
- libboost_date_time.a
- libboost_signals.a
- libboost_system.a
- libboost_thread.a
[4] Open the main qt file (GRT.pro) using QTCreator
[5] Select either the Debug or Release build
[6] Build the project, you should now be able to run the GRT GUI application
[7] If you want to deploy the application on other machines, you should run the python script which will setup the application for deployment: BuildApp.py

NOTE: So far, the GRT GUI has only been tested on OS X. Windows and Linux versions will be added soon.