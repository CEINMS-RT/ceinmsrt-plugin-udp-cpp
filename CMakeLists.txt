CMAKE_MINIMUM_REQUIRED (VERSION 3.10) 
PROJECT(Torque_Position_UDP_Simulink)


 IF(WIN32)
	IF(DEFINED ENV{XSD_HOME})
		set(XSD_EXECUTABLE "$ENV{XSD_HOME}/lib/Release/x64")
	endif()
	
		# No need to set anything here is installed in the default paths
	IF(DEFINED ENV{BOOST_INCLUDE_DIRS})
		set(BOOST_INCLUDE_DIRS $ENV{BOOST_INCLUDE_DIRS})
	else()
		set(BOOST_INCLUDE_DIRS "full/path/to/boost/lib64-msvc-14.1")
	endif()
	# message("boost include: ${BOOST_INCLUDE_DIRS}")

	IF(DEFINED ENV{Boost_LIBRARY})
		set(Boost_LIBRARY $ENV{Boost_LIBRARY})
	else()
		set(Boost_LIBRARY "full/path/to/boost")
	endif()
	# message("boost library: ${Boost_LIBRARY}")
	SET(Boost_USE_STATIC_LIBS ON)
 ENDIF(WIN32)



IF(UNIX)
    IF(CMAKE_COMPILER_IS_GNUCC)
         SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    ENDIF(CMAKE_COMPILER_IS_GNUCC)
	SET(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
	SET(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
	
	SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,,--add-stdcall-alias,--no-undefined,-allow-multiple-definition") # For Dynamic loading
	IF(CMAKE_VERSION VERSION_LESS "3.1")
		IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			SET(CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
		ENDIF()
	ELSE()
		SET(CMAKE_CXX_STANDARD 11)
	ENDIF()
ENDIF(UNIX)



FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} COMPONENTS timer chrono thread date_time program_options filesystem system unit_test_framework REQUIRED)
FIND_PACKAGE(XSD REQUIRED)
FIND_PACKAGE(XercesC REQUIRED)

INCLUDE_DIRECTORIES(include)

add_subdirectory(src)