#####################################################################################################################################################
# CMAKE List
#
# Tips:
#   IF STATEMENT EXAMPLE:
#       set(SRCS src.cpp)
#       if(WIN32)
#           set(SRCS ${SRCS} win_src.cpp)
#       elseif(UNIX)
#           set(SRCS ${SRCS} unix_src.cpp)
#       else()
#
#       endif()
#
#   Make with Debug (Default)
#       $ cd <path_to_cmakelist.txt>
#       $ mkdir build/debug
#       $ cd build/debug
#       $ cmake ../../ -DCMAKE_BUILD_TYPE=Debug
#
#   Make with Release
#       $ cd <path_to_cmakelist.txt>
#       $ mkdir build/release
#       $ cd build/release
#       $ cmake ../../ -DCMAKE_BUILD_TYPE=Release
#   
#####################################################################################################################################################

# BEGIN CMAKE Version (Required Line) ---------------------------------------------------------------------------------------------------------------
cmake_minimum_required (VERSION 3.3.2)
# END CMAKE Version ---------------------------------------------------------------------------------------------------------------------------------


# BEGIN Target Name ---------------------------------------------------------------------------------------------------------------------------------
SET(TARGET_NAME fixedPoint)
# END Target Name -----------------------------------------------------------------------------------------------------------------------------------


# BEGIN Project Name --------------------------------------------------------------------------------------------------------------------------------
PROJECT(${TARGET_NAME})
# END Project Name-----------------------------------------------------------------------------------------------------------------------------------


# BEGIN Set VS startup Project (Affects Windows only) -----------------------------------------------------------------------------------------------
#SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
# END Set VS startup Project ------------------------------------------------------------------------------------------------------------------------


# BEGIN Compiler Definitions ------------------------------------------------------------------------------------------------------------------------
#add_definitions(-DLINUX)
# END Compiler Definitions --------------------------------------------------------------------------------------------------------------------------


# BEGIN External CMake files ------------------------------------------------------------------------------------------------------------------------
# Make sure include and lib directories are relative to home dir of this cmake file
#ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR})
# END External CMake files --------------------------------------------------------------------------------------------------------------------------


# BEGIN Target Sources ------------------------------------------------------------------------------------------------------------------------------
FILE(GLOB TARGET_SRCS 
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.cu
)
# END Target Sources --------------------------------------------------------------------------------------------------------------------------------


# BEGIN Target Headers ------------------------------------------------------------------------------------------------------------------------------
FILE(GLOB TARGET_HEADERS
    ${PROJECT_SOURCE_DIR}/inc/*.h 
    ${PROJECT_SOURCE_DIR}/inc/*.hpp
    ${PROJECT_SOURCE_DIR}/inc/*.cuh
)
# END Target Sources --------------------------------------------------------------------------------------------------------------------------------


# BEGIN Library Settings ----------------------------------------------------------------------------------------------------------------------------
# For Visual studio this call must be before ADD_EXECUTABLE
#link_directories(
#    #<path_to_lib0>
#    #<path_to_lib1>
#    #<path_to_libN>
#)
# END Library Settings ------------------------------------------------------------------------------------------------------------------------------


# BEGIN Target Settings -----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(${TARGET_NAME} STATIC ${TARGET_SRCS} ${TARGET_HEADERS})
# END Target Settings -------------------------------------------------------------------------------------------------------------------------------


# BEGIN Build dependencies --------------------------------------------------------------------------------------------------------------------------
#add_dependencies(${TARGET_NAME} [<target-dependency>]...)
# END Build dependencies ----------------------------------------------------------------------------------------------------------------------------


# BEGIN Debug / Release Settings --------------------------------------------------------------------------------------------------------------------
IF(EXISTS "${PROJECT_SOURCE_DIR}/DEBUG_BUILD")
    SET(CMAKE_BUILD_TYPE Debug)
	SET(BUILD_DIR_SUFFIX "debug")
ELSEIF(EXISTS "${PROJECT_SOURCE_DIR}/RELEASE_BUILD")
    SET(CMAKE_BUILD_TYPE Release)
	SET(BUILD_DIR_SUFFIX "release")
ELSEIF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug)
	SET(BUILD_DIR_SUFFIX "debug")
ENDIF()
IF(WIN32)
	SET(CMAKE_CXX_FLAGS_DEBUG "/ZI /Od")
ELSE()
	SET(CMAKE_CXX_FLAGS_DEBUG "-g")
ENDIF()
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
MESSAGE("[" ${TARGET_NAME} "] Build Type: " ${CMAKE_BUILD_TYPE})
# END Debug / Release Settings ----------------------------------------------------------------------------------------------------------------------


# BEGIN Set Output Path for libs or bins ------------------------------------------------------------------------------------------------------------
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY_DEBUG 
                        ${PROJECT_SOURCE_DIR}/build/debug)
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY_RELEASE 
                        ${PROJECT_SOURCE_DIR}/build/release)
            
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
                        LIBRARY_OUTPUT_DIRECTORY_DEBUG 
                        ${PROJECT_SOURCE_DIR}/build/debug)
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
                        LIBRARY_OUTPUT_DIRECTORY_RELEASE
                        ${PROJECT_SOURCE_DIR}/build/release)
             
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG
                        ${PROJECT_SOURCE_DIR}/build/debug)
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE
                        ${PROJECT_SOURCE_DIR}/build/release)                     
# END Set Output Path for libs or bins --------------------------------------------------------------------------------------------------------------


# BEGIN Compiler Flags ------------------------------------------------------------------------------------------------------------------------------
TARGET_COMPILE_OPTIONS(${TARGET_NAME} PUBLIC 
    "-std=c++1y"
    "-Wall"
    "-fPIC"
    "-fexceptions"
)
# END Compiler Flags --------------------------------------------------------------------------------------------------------------------------------


# BEGIN Include directories -------------------------------------------------------------------------------------------------------------------------
TARGET_INCLUDE_DIRECTORIES(${TARGET_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/inc/
    /usr/local/include
    /usr/include/
)
# END Include directories ---------------------------------------------------------------------------------------------------------------------------


# BEGIN Library Settings ----------------------------------------------------------------------------------------------------------------------------
# Libraries to be linked against
#   For shared libraries make sure.....
#       WINDOWS: PATH env variable includes path(s) to dll's
#       LINUX:  LD_LIBRARY_PATH env variablel includes path(s) to so's
if(WIN32)
TARGET_LINK_LIBRARIES(${TARGET_NAME} PUBLIC
    #<lib0_name>.lib
    #<lib1_name>.lib
    #<libN_name>.lib
)
else()
TARGET_LINK_LIBRARIES(${TARGET_NAME} PUBLIC
    -L/usr/local/lib/
    -L/usr/lib/
)
endif()
# END Library Settings ------------------------------------------------------------------------------------------------------------------------------

