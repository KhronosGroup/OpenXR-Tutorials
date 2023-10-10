cmake_minimum_required(VERSION 3.22)

# Find Python
find_package(Python3 REQUIRED COMPONENTS Interpreter)
#message(STATUS "Python Version: ${Python3_VERSION}")

if (NOT Python3_FOUND)
	message(WARNING "Unable to find Python3 for reuse.")
	return()
elseif(${Python3_VERSION} VERSION_LESS 3.8)
	message(WARNING "reuse requires Python Version 3.8+.")
	return()
else()
	# Find/Install the reuse package
	execute_process(COMMAND pip3 show reuse 
		RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)
	if (NOT ${EXIT_CODE} EQUAL 0)
		execute_process(COMMAND pip3 install --user reuse)
		execute_process(COMMAND pip3 show reuse 
			RESULT_VARIABLE EXIT_CODE OUTPUT_QUIET)
		if (NOT ${EXIT_CODE} EQUAL 0)
			message(WARNING "Unable to find Python package: reuse.")
			return()
		endif()
	endif()
endif()

#Append PATH
if (WIN32)
	set(REUSE_PATH "%USERPROFILE%/AppData/Roaming/Python/Python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}/Scripts")
	#message(STATUS "Adding ${REUSE_PATH} to path.")
	LIST(APPEND CMAKE_PROGRAM_PATH ${REUSE_PATH})
elseif (LINUX)
	set(REUSE_PATH "~/.local/bin")
	#message(STATUS "Adding ${REUSE_PATH} to path.")
	LIST(APPEND CMAKE_PROGRAM_PATH "~/.local/bin")
endif()
#message(STATUS ${CMAKE_PROGRAM_PATH})

execute_process(COMMAND reuse lint 
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})