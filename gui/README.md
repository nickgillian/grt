###Gesture Recognition Toolkit GUI

####Building the GUI on OSX and Linux

1. Download and install QtCreator: http://qt-project.org/
2. Open Qt and try and build and run one of the examples to ensure it is installed correctly)
3. Download, and install boost: http://www.boost.org/users/download/ (the GUI assumes boost will be installed in /usr/local/include and /usr/local/lib on OSX and Linux machines)
4. Use the main GRT cmake file to build and install the GRT (you will find this in the build directory of the main project directory)
5. Open the main GRT qt file (GRT.pro) using QtCreator (Qt: File -> Open File or Project)
6. Select either the Debug or Release build (you might have to select 'Configure Project' the first time you open the project)
7. Check the GRT.pro file to make sure all the default include and lib directories match those on your system
8. Build the project
9. If the build is successful, press Run to launch the GUI
10. If you want to deploy the application on other machines, you should run the python script which will setup the application for deployment:

    python build-release_OSNAME.py
	
	
####Building on Windows

1. Download and install QTCreator: http://qt-project.org/ (ensure you include the MinGW compiler)
2. Open Qt and try and build and run one of the examples to ensure it is installed correctly)
3. Download boost 1_54_0 (newer versions of boost are much harder to compile on Windows): http://www.boost.org/users/history/
4. Install boost (see notes below)
5. Copy the main GRT project folder to: C:\grt
6. Open the main GRT qt file (GRT.pro) using QtCreator (Qt: File -> Open File or Project)
6. Select either the Debug or Release build (you might have to select 'Configure Project' the first time you open the project)
7. Check the GRT.pro file to make sure all the default include and lib directories match those on your system
8. Build the project
9. If the build is successful, press Run to launch the GUI

Note, so far this has only been tested on Windows 7 with Qt Creator 5.3 and boost 1_54_0.

####Building Boost on Windows
Building boost on Windows can be a major pain, this is the best options I've found so far:

1. Download boost 1_54_0 (the steps below do not work on newer versions of boost): http://www.boost.org/users/history/
2. Extract the downloaded boost zip file and copy the contents to the following location on your C drive: C:\boost\boost_1_54_0
3. Add mingw's path(e.g. C:\Qt\Tools\mingw\bin) to PATH variable by appending ';' then your path(e.g.: ";C:\Qt\Tools\mingw\bin") 
4. Open command prompt and cd to: C:\boost\boost_1_54_0\tools\build\v2\engine
5. Run: build.bat mingw
6. This will build two tools (b2.exe, bjam.exe) and place them in a sub directory: bin.ntx86
7. Copy both tools to: C:\boost\boost_1_54_0
8. Run: \b2 --toolset=mingw -j NUM_CORES _--build-type=complete stage (where NUM_CORES is the number of cores on your system)
9. If mingw is installed correctly, then you should see some log messages indicating that boost is being built - this takes a long time (several hours on my virtual machine)
10. This will build boost and install the libraries into: C:\boost\boost_1_54_0\stage\lib
11. Boost headers can be found at: C:\boost\boost_1_54_0\boost