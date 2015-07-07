## BuildRelease_OSX.py
# This python script builds the release version of the GRT GUI. It also runs the QT macdeploy tool which wraps up any libraries or external dependices needed for the
# GUI within the application itself so the app can run on other OS X machines without having to install QT or the GRT.
# Before you run this script, you should open the GRT QT project file with QT creator and build the release application (this will generate a make file for your machine)
# When setting up the build, you should set the name of the build directory to build-release
# You can then use this script to build the deployment version which can be used on other machines
# To use this script you should change the QT lib and bin paths to the locaction on your local machine
# You can then build the release version by running this file from the terminal
#You should run this script by passing in the qt bin directory as the first argument, for example:
#python build-release_osx.py /Users/ngillian/Qt/5.4/clang_64/bin/

import os
import re
import subprocess
from optparse import OptionParser

#Parse the command line options, the first option should be the qt bin directory
parser = OptionParser()
(options, args) = parser.parse_args()

if( len( args ) < 1 ):
    raise NameError( "Missing Argument: You need to specify the qt compiler bin directory on your system as the first argument, check this file for more info" ) 

#NOTE: on my machine, the qt bin directory is: "/Users/n.gillian/Qt/5.2.1/clang_64/bin/"
macDeployDir = args[0]

#Set the main directory
mainDir = "build-release/"
appDir = mainDir + "GRT.app/"

#Check to see if the PlugIns directory exists, if so then remove it
if( os.path.exists( appDir + "Contents/PlugIns" ) == True ):
	print "Removing PlugIns directory..."
	os.system( "rm -rf " + appDir + "Contents/PlugIns" )	

#Check to see if the config file exists, if so then remove it
if( os.path.exists( appDir + "Contents/Resources/qt.conf" ) == True ):
	print "Removing config file..."
	os.system( "rm " + appDir + "Contents/Resources/qt.conf" )	

#Check to see if the config file exists, if so then remove it
if( os.path.exists( appDir + "Contents/Resources/qt.conf" ) == True ):
	print "Removing config file..."
	os.system( "rm " + appDir + "Contents/Resources/qt.conf" )
	
#Run qmake
os.system( "qmake GRT/GRT.pro -config release -r -spec macx-clang CONFIG+=x86_64" )
	
#Build the application
print "Building application..."
os.system( "make -C " + mainDir + " clean " )
os.system( "make -C " + mainDir )

#Copy the GRT library into the GUI application for deployment
os.system( "cp -fv " + "/usr/local/lib/" + "libgrt.dylib " + "/usr/lib/" )

#Run the mac deploy tool so the application can be run on other machines
print "Running mac deployment..."
os.system( macDeployDir + "macdeployqt" + " " + mainDir + "GRT.app" + " -verbose=1" )

#Run the otool to make sure the libraries are now pointing to local copies
os.system( "otool -L " + mainDir + "GRT.app/Contents/MacOS/GRT" )
