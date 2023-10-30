# Locate the raylib library
#
# This module defines the following variables:
#
# RAYLIB_LIBRARY the name of the library;
# RAYLIB_INCLUDE_DIR where to find raylib include files.
# RAYLIB_FOUND true if both the RAYLIB_LIBRARY and RAYLIB_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called RAYLIB_ROOT which points to the root of the raylib library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _raylib_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/windows/raylib/include"
)
set( _raylib_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/windows/raylib/lib"
)

# Check environment for root search directory
set( _raylib_ENV_ROOT $ENV{RAYLIB_ROOT} )
if( NOT RAYLIB_ROOT AND _raylib_ENV_ROOT )
	set(RAYLIB_ROOT ${_raylib_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( RAYLIB_ROOT )
	list( INSERT _raylib_HEADER_SEARCH_DIRS 0 "${RAYLIB_ROOT}/include" )
	list( INSERT _raylib_LIB_SEARCH_DIRS 0 "${RAYLIB_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(RAYLIB_INCLUDE_DIR "raylib.h"
PATHS ${_raylib_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(RAYLIB_LIBRARY NAMES raylib RAYLIB
PATHS ${_raylib_LIB_SEARCH_DIRS} )
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RAYLIB DEFAULT_MSG
RAYLIB_LIBRARY RAYLIB_INCLUDE_DIR)
