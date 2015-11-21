cmake_minimum_required(VERSION 2.8)

project(GRT C CXX)

# GRT library
set(GRT_LIB_NAME grt)

#set the main options
option(BUILD_EXAMPLES “build-examples” ON)
option(BUILD_TOOLS “build-tools” ON)
option(BUILD_SHARED_LIB "build-shared-lib" ON)
option(ENABLE_CXX11_SUPPORT "enable-c++11-support" ON)
option(EXCLUDE_FROM_INSTALL "exclude-from-install" OFF)

#Setup if the library should be built as static or shared, default is SHARED, turning shared off will make a static build
if(BUILD_SHARED_LIB MATCHES ON)
	set(LIB_BUILD_TYPE SHARED)
	set(LIB_BUILD_TYPE_INFO "shared")
	set(BUILD_STATIC_LIB OFF) #Make sure the static lib is off
else() #setup for static build
    set(LIB_BUILD_TYPE STATIC)
    set(LIB_BUILD_TYPE_INFO "static")
    set(BUILD_SHARED_LIB OFF) #Make sure the shared lib is off
endif()

#If C++11 is enabled, then define the relevant flag
if( ENABLE_CXX11_SUPPORT MATCHES ON)
    add_definitions(-DGRT_CXX11_ENABLED)
endif()

#Add any custom OS definitions
if(MSVC)
    message(STATUS "Building " ${LIB_BUILD_TYPE_INFO} " library for Windows")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")
endif()

#Add any custom OSX definitions
if(APPLE)
    message(STATUS "Building " ${LIB_BUILD_TYPE_INFO} " library for OSX")
    configure_file(grt.pc.in ${CMAKE_CURRENT_BINARY_DIR}/grt.pc @ONLY)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")

    #Add C++11 and multithreading support
    if( ENABLE_CXX11_SUPPORT MATCHES ON )
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
    endif()
else()
	
#Add any custom Linux definitions
if(UNIX) 
    message(STATUS "Building " ${LIB_BUILD_TYPE_INFO}  " library for UNIX/Linux")
    configure_file(grt.pc.in ${CMAKE_CURRENT_BINARY_DIR}/grt.pc @ONLY)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")

    #Add C++11 and multithreading support
    if( ENABLE_CXX11_SUPPORT MATCHES ON )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
    endif()

endif()

if(MINGW)
    message(STATUS "Building " ${LIB_BUILD_TYPE_INFO}  " library for Windows/MinGW")
    configure_file(grt.pc.in ${CMAKE_CURRENT_BINARY_DIR}/grt.pc @ONLY)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")

    #Add C++11 and multithreading support
    if( ENABLE_CXX11_SUPPORT MATCHES ON )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
    endif()
endif()

endif() #end of else(APPLE)

#Set the main directory locations
set(GRT_SRC_DIR
    ${PROJECT_SOURCE_DIR}/../GRT
)

set(GRT_EXAMPLES_DIR
    ${PROJECT_SOURCE_DIR}/../examples
)

set(GRT_EXAMPLES_OUTPUT_DIR
    ${PROJECT_SOURCE_DIR}/examples
)

set(GRT_TOOLS_DIR
    ${PROJECT_SOURCE_DIR}/../tools
)

#Add all the project files
file(GLOB GRT_CLASSIFICATION_MODULES
    "${GRT_SRC_DIR}/ClassificationModules/AdaBoost/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/AdaBoost/WeakClassifiers/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/ANBC/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/BAG/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/CNN/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/DecisionTree/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/DTW/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/FiniteStateMachine/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/GMM/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/KNN/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/LDA/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/ParticleClassifier/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/RandomForests/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/Softmax/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/SVM/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/SVM/LIBSVM/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/HMM/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/MinDist/*.cpp"
    "${GRT_SRC_DIR}/ClassificationModules/SwipeDetector/*.cpp"
)

file(GLOB GRT_CLUSTERING_MODULES
    "${GRT_SRC_DIR}/ClusteringModules/ClusterTree/*.cpp"
    "${GRT_SRC_DIR}/ClusteringModules/GaussianMixtureModels/*.cpp"
    "${GRT_SRC_DIR}/ClusteringModules/KMeans/*.cpp"
    "${GRT_SRC_DIR}/ClusteringModules/HierarchicalClustering/*.cpp"
    "${GRT_SRC_DIR}/ClusteringModules/SelfOrganizingMap/*.cpp"
)

file(GLOB GRT_CONTEXT_MODULES
    "${GRT_SRC_DIR}/ContextModules/*.cpp"
)

file(GLOB GRT_DATA_STRUCTURES
    "${GRT_SRC_DIR}/DataStructures/*.cpp"
)

file(GLOB GRT_FEATURE_EXTRACTION_MODULES
    "${GRT_SRC_DIR}/FeatureExtractionModules/FFT/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/KMeansFeatures/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/KMeansQuantizer/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/MovementIndex/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/MovementTrajectoryFeatures/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/RBMQuantizer/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/SOMQuantizer/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/TimeDomainFeatures/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/TimeseriesBuffer/*.cpp"
    "${GRT_SRC_DIR}/FeatureExtractionModules/ZeroCrossingCounter/*.cpp"
)

file(GLOB GRT_CORE_ALGORITHMS
    "${GRT_SRC_DIR}/CoreAlgorithms/BernoulliRBM/*.cpp"
    "${GRT_SRC_DIR}/CoreAlgorithms/MovementDetector/*.cpp"
    "${GRT_SRC_DIR}/CoreAlgorithms/PrincipalComponentAnalysis/*.cpp"
    "${GRT_SRC_DIR}/CoreAlgorithms/Tree/*.cpp"
)

file(GLOB GRT_PIPELINE
    "${GRT_SRC_DIR}/CoreModules/*.cpp"
)

file(GLOB GRT_POST_PROCESSING_MODULES
    "${GRT_SRC_DIR}/PostProcessingModules/*.cpp"
)

file(GLOB GRT_PRE_PROCESSING_MODULES
    "${GRT_SRC_DIR}/PreProcessingModules/*.cpp"
)

file(GLOB GRT_REGRESSION_MODULES
    "${GRT_SRC_DIR}/RegressionModules/ArtificialNeuralNetworks/MLP/*.cpp"
    "${GRT_SRC_DIR}/RegressionModules/LinearRegression/*.cpp"
    "${GRT_SRC_DIR}/RegressionModules/LogisticRegression/*.cpp"
    "${GRT_SRC_DIR}/RegressionModules/MultidimensionalRegression/*.cpp"
    "${GRT_SRC_DIR}/RegressionModules/RegressionTree/*.cpp"
)

file(GLOB GRT_UTIL
    "${GRT_SRC_DIR}/Util/*.cpp"
)

set(GRT_SRC
    ${GRT_CLASSIFICATION_MODULES}
    ${GRT_CLUSTERING_MODULES}
    ${GRT_CONTEXT_MODULES}
    ${GRT_DATA_STRUCTURES}
    ${GRT_FEATURE_EXTRACTION_MODULES}
    ${GRT_CORE_ALGORITHMS}
    ${GRT_PIPELINE}
    ${GRT_POST_PROCESSING_MODULES}
    ${GRT_PRE_PROCESSING_MODULES}
    ${GRT_REGRESSION_MODULES}
    ${GRT_UTIL}
)

file(GLOB_RECURSE GRT_HEADERS
    "${GRT_SRC_DIR}/*.h"
)

include_directories(
    "${GRT_SRC_DIR}"
)

add_library(${GRT_LIB_NAME} 
            ${LIB_BUILD_TYPE}
            ${GRT_SRC}
            ${GRT_HEADERS}
)

if(NOT EXCLUDE_FROM_INSTALL)
	install(
		TARGETS ${GRT_LIB_NAME}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
	)

	install(
		DIRECTORY "${GRT_SRC_DIR}/"
		DESTINATION include/GRT
		FILES_MATCHING PATTERN *.h
	)
endif()

# GRT examples
if( BUILD_EXAMPLES )
	function(grt_add_example _example_name _example_sub_directory)
	    
            #Search for any source files in the example directory
            file(GLOB sources
	        "${GRT_EXAMPLES_DIR}/${_example_sub_directory}/${_example_name}/*.cpp"
	        "${GRT_EXAMPLES_DIR}/${_example_sub_directory}/${_example_name}/*.h"
	    )

            #If there are source files for the example, then setup the example application
        if( sources )
            #Add the example as a new executable application
	        add_executable(${_example_name}
	            ${sources}
	        )

            #Include the main GRT include directory
            include_directories( "${GRT_EXAMPLES_DIR}/.." )

            #Link against the main GRT library 
            target_link_libraries(${_example_name}
	            ${GRT_LIB_NAME}
	        )

            #Copy the example application to the main examples directory after a successful build
            add_custom_command(TARGET ${_example_name}
				                POST_BUILD
                                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${_example_name}> ${GRT_EXAMPLES_OUTPUT_DIR}/${_example_name}
				                COMMENT "Copying ${_example_name} to examples directory"
            )

            #Guess the training data name that we want to copy
            string(REPLACE
                "Example"           # match string
                "TrainingData"      # replace string
                TRAINING_DATA       # output
                ${_example_name}    # input
            )

            #If a data file exists for this example, copy it to the main examples directory after a successful build
            if(EXISTS "${GRT_EXAMPLES_DIR}/${_example_sub_directory}/${_example_name}/${TRAINING_DATA}.grt")
                add_custom_command(
                    TARGET ${_example_name}
  					POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy 
					"${GRT_EXAMPLES_DIR}/${_example_sub_directory}/${_example_name}/${TRAINING_DATA}.grt" #copy target
                    "${GRT_EXAMPLES_OUTPUT_DIR}/${TRAINING_DATA}.grt"					 #copy destination
					COMMENT "Copying ${TRAINING_DATA}.grt to examples directory"
                )
            endif()
	    endif()
	endfunction()

	macro(grt_get_sub_directory_list _result _source_directory)
	    file(GLOB children
	        RELATIVE "${_source_directory}"
	        "${_source_directory}/*"
	    )

	    set(${_result} "")

	    foreach(child ${children})
	        if(IS_DIRECTORY "${_source_directory}/${child}")
	            list(APPEND ${_result} ${child})
	        endif()
	    endforeach()
	endmacro()

	grt_get_sub_directory_list(GRT_EXAMPLES_SUB_DIRECTORIES
	    "${GRT_EXAMPLES_DIR}"
	)

	foreach(GRT_EXAMPLES_SUB_DIRECTORY ${GRT_EXAMPLES_SUB_DIRECTORIES})
	    grt_get_sub_directory_list(GRT_EXAMPLES
	        "${GRT_EXAMPLES_DIR}/${GRT_EXAMPLES_SUB_DIRECTORY}"
	    )

	    foreach(GRT_EXAMPLE ${GRT_EXAMPLES})
	        grt_add_example(
	            ${GRT_EXAMPLE}
	            ${GRT_EXAMPLES_SUB_DIRECTORY}
	        )
	    endforeach()
	endforeach()
endif() #BUILD_EXAMPLES

#GRT Tool
if( BUILD_TOOLS )
    #Get a list of the tools
    file(GLOB GRT_TOOLS "${GRT_TOOLS_DIR}/*.cpp")

    foreach(GRT_TOOL ${GRT_TOOLS})
        #Remove the file extension
        GET_FILENAME_COMPONENT(EXE ${GRT_TOOL} NAME_WE) 

        #Add the application tools
        add_executable (${EXE} ${GRT_TOOL})

        include_directories( "${GRT_EXAMPLES_DIR}/.." )

        #Link the tools against the GRT library
        target_link_libraries(${EXE} ${GRT_LIB_NAME})

        if(NOT EXCLUDE_FROM_INSTALL)
            install(
                TARGETS ${EXE}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib
            )
        endif()
    endforeach()

endif() #BUILD_TOOLS

if(UNIX AND NOT EXCLUDE_FROM_INSTALL)
    set(CMAKE_INSTALL_LIBDIR "lib/${CMAKE_LIBRARY_ARCHITECTURE}" CACHE PATH "Output directory for libraries")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/grt.pc                         
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig                               
    )
endif()

