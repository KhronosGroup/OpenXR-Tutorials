# Copyright 2023, The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

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
	file(REAL_PATH "~/AppData/Roaming/Python/Python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}/Scripts" REUSE_PATH EXPAND_TILDE)
elseif (LINUX)
	set(REUSE_PATH "~/.local/bin")
endif()

message(STATUS "${REUSE_PATH}/reuse lint")
execute_process(COMMAND ${REUSE_PATH}/reuse lint 
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})