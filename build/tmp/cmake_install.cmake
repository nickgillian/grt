# Install script for directory: /home/lukas/Dokumente/BA/GRT/build

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/libgrt.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/GRT" TYPE DIRECTORY FILES "/home/lukas/Dokumente/BA/GRT/build/../GRT/" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovementIndexExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovementIndexExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovementIndexExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MovementIndexExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MovementIndexExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovementIndexExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovementIndexExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovementIndexExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTFeaturesExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTFeaturesExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTFeaturesExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/FFTFeaturesExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/FFTFeaturesExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTFeaturesExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTFeaturesExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTFeaturesExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/FFTExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/FFTExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/FFTExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/KMeansQuantizerExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/KMeansQuantizerTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/FeatureExtractionModules/KMeansQuantizerExample/KMeansQuantizerTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/PrincipalComponentAnalysisExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/PrincipalComponentAnalysisExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/PrincipalComponentAnalysisExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/PrincipalComponentAnalysisExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/PrincipalComponentAnalysisExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/PrincipalComponentAnalysisExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/PrincipalComponentAnalysisExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/PrincipalComponentAnalysisExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/ClassLabelFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelFilterTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/PostprocessingModulesExamples/ClassLabelFilterExample/ClassLabelFilterTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/ClassLabelTimeoutFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelTimeoutFilterTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/PostprocessingModulesExamples/ClassLabelTimeoutFilterExample/ClassLabelTimeoutFilterTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/ClassLabelChangeFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClassLabelChangeFilterTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/PostprocessingModulesExamples/ClassLabelChangeFilterExample/ClassLabelChangeFilterTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MinDistExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MinDistExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MinDistExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MinDistExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MinDistExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MinDistExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MinDistExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MinDistExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MinDistTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/MinDistExample/MinDistTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GMMExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GMMExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GMMExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/GMMExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/GMMExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GMMExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GMMExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GMMExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/GMMTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/GMMExample/GMMTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ANBCExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ANBCExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ANBCExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ANBCExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/ANBCExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ANBCExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ANBCExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ANBCExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ANBCTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/ANBCExample/ANBCTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/RandomForestsExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/RandomForestsTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/RandomForestsExample/RandomForestsTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVMExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVMExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVMExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/SVMExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/SVMExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVMExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVMExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVMExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/SVMTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/SVMExample/SVMTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HMMExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HMMExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HMMExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/HMMExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/HMMExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HMMExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HMMExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HMMExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/HMMTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/HMMExample/HMMTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DTWExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DTWExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DTWExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/DTWExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/DTWExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DTWExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DTWExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DTWExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/DTWTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/DTWExample/DTWTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/BAGExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/BAGExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/BAGExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/BAGExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/BAGExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/BAGExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/BAGExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/BAGExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/BAGTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/BAGExample/BAGTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/DecisionTreeExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/DecisionTreeTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/DecisionTreeExample/DecisionTreeTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KNNExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KNNExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KNNExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/KNNExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/KNNExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KNNExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KNNExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KNNExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/KNNTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/KNNExample/KNNTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/SoftmaxExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/SoftmaxTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/SoftmaxExample/SoftmaxTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/AdaBoostExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/AdaBoostTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/ClassificationModulesExamples/AdaBoostExample/AdaBoostTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RegressionDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RegressionDataExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RegressionDataExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/RegressionDataExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/RegressionDataExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RegressionDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RegressionDataExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RegressionDataExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UnlabelledDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UnlabelledDataExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UnlabelledDataExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/UnlabelledDataExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/UnlabelledDataExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UnlabelledDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UnlabelledDataExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UnlabelledDataExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/TimeSeriesClassificationDataExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataStreamExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataStreamExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataStreamExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataStreamExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/TimeSeriesClassificationDataStreamExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataStreamExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataStreamExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/TimeSeriesClassificationDataStreamExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassificationDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassificationDataExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassificationDataExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClassificationDataExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/ClassificationDataExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassificationDataExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassificationDataExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClassificationDataExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LowPassFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LowPassFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LowPassFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/LowPassFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/LowPassFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LowPassFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LowPassFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LowPassFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DerivativeExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DerivativeExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DerivativeExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/DerivativeExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/DerivativeExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DerivativeExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DerivativeExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DerivativeExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SavitskyGolayFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SavitskyGolayFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SavitskyGolayFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/SavitskyGolayFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/SavitskyGolayFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SavitskyGolayFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SavitskyGolayFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SavitskyGolayFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DeadZoneExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DeadZoneExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DeadZoneExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/DeadZoneExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/DeadZoneExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DeadZoneExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DeadZoneExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DeadZoneExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovingAverageFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovingAverageFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovingAverageFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MovingAverageFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MovingAverageFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovingAverageFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovingAverageFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MovingAverageFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DoubleMovingAverageFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DoubleMovingAverageFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DoubleMovingAverageFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/DoubleMovingAverageFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/DoubleMovingAverageFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DoubleMovingAverageFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DoubleMovingAverageFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/DoubleMovingAverageFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MedianFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MedianFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MedianFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MedianFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MedianFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MedianFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MedianFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MedianFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HighPassFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HighPassFilterExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HighPassFilterExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/HighPassFilterExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/HighPassFilterExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HighPassFilterExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HighPassFilterExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HighPassFilterExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HelloWorld" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HelloWorld")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HelloWorld"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/HelloWorld")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/HelloWorld")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HelloWorld" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HelloWorld")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/HelloWorld")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GettingStarted" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GettingStarted")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GettingStarted"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/GettingStarted")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/GettingStarted")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GettingStarted" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GettingStarted")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GettingStarted")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MachineLearning101" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MachineLearning101")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MachineLearning101"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MachineLearning101")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MachineLearning101")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MachineLearning101" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MachineLearning101")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MachineLearning101")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomFeatureExtraction" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomFeatureExtraction")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomFeatureExtraction"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/CustomFeatureExtraction")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/CustomFeatureExtraction")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomFeatureExtraction" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomFeatureExtraction")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomFeatureExtraction")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomMakefile" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomMakefile")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomMakefile"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/CustomMakefile")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/CustomMakefile")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomMakefile" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomMakefile")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CustomMakefile")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/RandomExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/RandomExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/RandomExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MatrixExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MatrixExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CircularBufferExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CircularBufferExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CircularBufferExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/CircularBufferExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/CircularBufferExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CircularBufferExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CircularBufferExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/CircularBufferExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UtilExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UtilExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UtilExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/UtilExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/UtilExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UtilExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UtilExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/UtilExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixDoubleExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixDoubleExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixDoubleExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MatrixDoubleExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MatrixDoubleExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixDoubleExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixDoubleExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MatrixDoubleExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVDExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVDExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVDExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/SVDExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/SVDExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVDExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVDExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/SVDExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GaussianMixtureModelsExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GaussianMixtureModelsExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GaussianMixtureModelsExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/GaussianMixtureModelsExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/GaussianMixtureModelsExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GaussianMixtureModelsExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GaussianMixtureModelsExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/GaussianMixtureModelsExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClusterTreeExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClusterTreeExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClusterTreeExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/ClusterTreeExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/ClusterTreeExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClusterTreeExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClusterTreeExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/ClusterTreeExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/KMeansExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/KMeansExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/KMeansExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MLPRegressionExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MLPRegressionTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/RegressionModulesExamples/MLPRegressionExample/MLPRegressionTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/LogisticRegressionExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/LogisticRegressionTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/RegressionModulesExamples/LogisticRegressionExample/LogisticRegressionTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/MultidimensionalRegressionExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/MultidimensionalRegressionTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/RegressionModulesExamples/MultidimensionalRegressionExample/MultidimensionalRegressionTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionExample"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE EXECUTABLE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/LinearRegressionExample")
  if(EXISTS "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionExample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionExample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/lukas/Dokumente/BA/GRT/build/examples/LinearRegressionTrainingData.grt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/lukas/Dokumente/BA/GRT/build/examples" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/../examples/RegressionModulesExamples/LinearRegressionExample/LinearRegressionTrainingData.grt")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/x86_64-linux-gnu/pkgconfig" TYPE FILE FILES "/home/lukas/Dokumente/BA/GRT/build/tmp/grt.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

file(WRITE "/home/lukas/Dokumente/BA/GRT/build/tmp/${CMAKE_INSTALL_MANIFEST}" "")
foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
  file(APPEND "/home/lukas/Dokumente/BA/GRT/build/tmp/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
endforeach()
