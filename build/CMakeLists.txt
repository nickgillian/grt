cmake_minimum_required(VERSION 2.8)

project(GRT C CXX)

# GRT library
set(GRT_LIB_NAME grt)

#set the main options, you can control these dynamically via the command line by passing in the option -D OPTION_NAME=ON
#e.g., cmake .. -DBUILD_TESTS=ON
option(BUILD_TESTS “build-unit-tests” ON)
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

#If static lib is enabled, then define the relevant flag
if( BUILD_STATIC_LIB MATCHES ON )
	add_definitions(-DGRT_STATIC_LIB)
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
set(GRT_SRC_DIR ${PROJECT_SOURCE_DIR}/../GRT)
set(GRT_THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/../third_party)
set(GRT_UNIT_TEST_DIR ${PROJECT_SOURCE_DIR}/../tests)
set(GTEST_DIR ${GRT_THIRD_PARTY_DIR}/gtest)
set(GTEST_INCLUDE_DIR ${GTEST_DIR}/googletest/include)
set(GTEST_LIB_DIR ${PROJECT_SOURCE_DIR}/gtest/googlemock)
set(GRT_EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/../examples)
set(GRT_EXAMPLES_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/examples)
set(GRT_TOOLS_DIR ${PROJECT_SOURCE_DIR}/../tools)

#Add all the project files
file(GLOB_RECURSE GRT_CLASSIFICATION_MODULES
    "${GRT_SRC_DIR}/ClassificationModules/*.cpp"
)

file(GLOB_RECURSE GRT_CLUSTERING_MODULES
    "${GRT_SRC_DIR}/ClusteringModules/*.cpp"
)

file(GLOB GRT_CONTEXT_MODULES
    "${GRT_SRC_DIR}/ContextModules/*.cpp"
)

file(GLOB GRT_DATA_STRUCTURES
    "${GRT_SRC_DIR}/DataStructures/*.cpp"
)

file(GLOB_RECURSE GRT_FEATURE_EXTRACTION_MODULES
    "${GRT_SRC_DIR}/FeatureExtractionModules/*.cpp"
)

file(GLOB_RECURSE GRT_CORE_ALGORITHMS
    "${GRT_SRC_DIR}/CoreAlgorithms/*.cpp"
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

file(GLOB_RECURSE GRT_REGRESSION_MODULES
    "${GRT_SRC_DIR}/RegressionModules/*.cpp"
)

file(GLOB GRT_UTIL
    "${GRT_SRC_DIR}/Util/*.cpp"
)

set(GRT_SRC
    ${GRT_UTIL}
    ${GRT_DATA_STRUCTURES}
    ${GRT_CORE_ALGORITHMS}
    ${GRT_CORE_MODULES}
    ${GRT_CLASSIFICATION_MODULES}
    ${GRT_CLUSTERING_MODULES}
    ${GRT_CONTEXT_MODULES}
    ${GRT_FEATURE_EXTRACTION_MODULES}
    ${GRT_PIPELINE}
    ${GRT_POST_PROCESSING_MODULES}
    ${GRT_PRE_PROCESSING_MODULES}
    ${GRT_REGRESSION_MODULES}
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

# GRT Unit Tests
if( BUILD_TESTS )
    ENABLE_TESTING()

    #Add a command that will automatically build the unit testing tools for us
    set(BUILD_GTEST
        ON
        CACHE
        BOOL
        "Builds the googletest subproject"
    )
    set(BUILD_GMOCK
        OFF
        CACHE
        BOOL
        "Builds the googlemock subproject"
    )
    set(gtest_force_shared_crt
        ON
        CACHE
        BOOL
        "Use shared (DLL) run-time lib even when Google Test is built as static lib."
    )

    add_subdirectory(
        "${GTEST_DIR}"
        "${PROJECT_BINARY_DIR}/third_party/gtest"
        EXCLUDE_FROM_ALL
    )

    #Add the unit tests
    message(STATUS "Adding unit tests...")
    file(GLOB_RECURSE GRT_UNIT_TESTS "${GRT_UNIT_TEST_DIR}/*.cpp")
    foreach(unit_test ${GRT_UNIT_TESTS})
        message(STATUS "Adding unit test: " ${unit_test})

        #Remove the file extension
        GET_FILENAME_COMPONENT(unit_test_name ${unit_test} NAME_WE) 

        #Add the example as a new executable application
        add_executable(${unit_test_name} ${unit_test})

        #Include the main GRT include directory
        include_directories( "${GRT_SRC_DIR}" )

        #Include the gtest parent directory
        include_directories( "${GRT_THIRD_PARTY_DIR}/gtest/googletest/include" )

        #Link against the main GRT library 
        target_link_libraries(${unit_test_name} ${GRT_LIB_NAME} gtest)

        #Add the test
        add_test( ${unit_test_name} ${unit_test_name} )
        
        #add_custom_command(TARGET ${unit_test_name} POST_BUILD COMMAND ${unit_test_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Running ${unit_test_name}" VERBATIM)
    endforeach()
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

