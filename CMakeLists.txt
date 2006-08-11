PROJECT(TSP C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.2)

# Use TSP specific CMake modules first
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/make)
# Load TSP Specific CMake macros (TSP_XSCRIPT, TSP_XINCLUDE)
INCLUDE(TSPCMakeTools)

OPTION(BUILD_PROVIDER
  "Build providers" ON)

OPTION(BUILD_CONSUMER
  "Build consumers" ON)

OPTION(BUILD_XMLRPC
  "Build with XMLRPC support" OFF)

OPTION(BUILD_API_DOC
  "Build doxygen documentation" ON)

OPTION(BUILD_BB_KERNEL_MODULE
  "Build the in-kernel bb" OFF)

SET(PACKAGE_NAME      "tsp")
SET(PACKAGE_VERSION   "0.8.cvs")
SET(PACKAGE_BUGREPORT "tsp@astrium.eads.net")

MESSAGE(STATUS "Configuring ${PACKAGE_NAME} version ${PACKAGE_VERSION}")
INCLUDE(CheckFunctionExists)
#INCLUDE(CMakePrintSystemInformation)

CHECK_FUNCTION_EXISTS(nanosleep HAS_NANOSLEEP)
IF(HAS_NANOSLEEP)
  ADD_DEFINITIONS(-DTSP_SYSTEM_HAVE_NANOSLEEP)
ENDIF(HAS_NANOSLEEP)


INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src/core/include
  ${CMAKE_SOURCE_DIR}/src/core/
  ${CMAKE_SOURCE_DIR}/src/core/driver
  ${CMAKE_SOURCE_DIR}/src/core/common
  ${CMAKE_SOURCE_DIR}/src/core/rpc
  ${CMAKE_SOURCE_DIR}/src/core/xmlrpc
  ${CMAKE_SOURCE_DIR}/src/core/misc_utils
  ${CMAKE_SOURCE_DIR}/src/util/libpages
  ${CMAKE_SOURCE_DIR}/src/util/libutil
  ${CMAKE_SOURCE_DIR}/src/core/ctrl_init
  ${CMAKE_SOURCE_DIR}/src/core/ctrl
  ${CMAKE_SOURCE_DIR}/src/util/libtspcfg/
  )

#
# Be nice use FIND PACKAGE for searching library
# if there is no predefined CMake FindXXX module
# for your needs please write one
# and put it in {TSP_BASE}/make directory
# see  
# http://www.cmake.org/Wiki/CMake_HowToFindInstalledSoftware
# for building your own CMake module
#

################ Open GL Check ####################
MESSAGE(STATUS "Looking for OpenGL")
FIND_PACKAGE(OpenGL)
IF (OPENGL_FOUND)
  MESSAGE(STATUS "Looking for OpenGL - found : ${OPENGL_gl_LIBRARY}")
  GET_FILENAME_COMPONENT(GLPATH ${OPENGL_gl_LIBRARY} PATH)
  LINK_DIRECTORIES(${GLPATH})
ELSE (OPENGL_FOUND)
  MESSAGE(STATUS "Looking for OpenGL - not found")
ENDIF (OPENGL_FOUND)

################ GLU Check ####################
MESSAGE(STATUS "Looking for GLU")
FIND_PACKAGE(GLU)
IF (OPENGL_GLU_FOUND)
  MESSAGE(STATUS "Looking for GLU - found : ${OPENGL_glu_LIBRARY}")
ELSE (OPENGL_GLU_FOUND)
  MESSAGE(STATUS "Looking for GLU - not found")
ENDIF (OPENGL_GLU_FOUND)

################ GLUT Check ####################
MESSAGE(STATUS "Looking for GLUT")
FIND_PACKAGE(GLUT)
IF (GLUT_FOUND)
  MESSAGE(STATUS "Looking for GLUT - found : ${GLUT_glut_LIBRARY}")
ELSE (GLUT_FOUND)
  MESSAGE(STATUS "Looking for GLUT - not found")
ENDIF (GLUT_FOUND)

################ LEX/YACC Check ####################
FIND_PACKAGE(LexYacc)
################ GTK+1 Check ####################
FIND_PACKAGE(GTK)
################ XML2 Check ####################
FIND_PACKAGE(XML2)

################ FUSE Check ####################
FIND_PACKAGE(FUSE)
IF(FUSE_FOUND)
  MESSAGE(STATUS "FUSE consumer enabled")
ELSE(FUSE_FOUND)
  MESSAGE(STATUS "FUSE consumer disabled")
ENDIF(FUSE_FOUND)

################ xmlrpc-c check ####################
IF (BUILD_XMLRPC)
    FIND_PACKAGE(XMLRPCC REQUIRED)
    FIND_PACKAGE(libwww REQUIRED)
ENDIF (BUILD_XMLRPC)

################ BUILD related config ############################
IF (NOT CMAKE_BUILD_TYPE)
 SET(CMAKE_BUILD_TYPE "Debug")
ENDIF (NOT CMAKE_BUILD_TYPE)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
  ADD_DEFINITIONS(-DUSE_STRACE)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(LIBRARY_OUTPUT_PATH    ${CMAKE_SOURCE_DIR}/exec/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/bin)

SET(DIST_DIR ${PROJECT_SOURCE_DIR}/dist)
MARK_AS_ADVANCED(DIST_DIR)

LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

################ INSTALL related config ############################
SET(CMAKE_INSTALL_PREFIX /opt/${PACKAGE_NAME}-${PACKAGE_VERSION})

ADD_CUSTOM_TARGET(cmake_clean
  COMMAND sh -c "rm -rf ${CMAKE_SOURCE_DIR}/exec"
  COMMAND sh -c "rm -f  ${CMAKE_SOURCE_DIR}/CMakeCache.txt"
  COMMAND sh -c "${CMAKE_SOURCE_DIR}/src/scripts/purge_CMakeFiles.sh ${CMAKE_SOURCE_DIR}")

################ TSP SDK related config ###########################
SET(HOST_TARGET ${CMAKE_SYSTEM_NAME})
SET(TSP_BASE  ${PROJECT_SOURCE_DIR})

MARK_AS_ADVANCED(
  HOST_TARGET
  TSP_BASE)

TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp_dev.login.in
  src/scripts/tsp_dev.login
  @ONLY)

TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp_dev.login.sh.in 
  src/scripts/tsp_dev.login.sh 
  @ONLY)

TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp.pc.in
  src/scripts/tsp.pc
  @ONLY)

## Now add subdirectories
ADD_SUBDIRECTORY(external)
ADD_SUBDIRECTORY(src)
MESSAGE(STATUS "Configuring ${PACKAGE_NAME} version ${PACKAGE_VERSION}...Done.")

MESSAGE("***********************************************************")
MESSAGE("*  TSP has been successfully configured.                  *")
MESSAGE("*  you should now do either                               *")
MESSAGE("*     source src/scripts/tsp_dev.login  [csh/tcsh]        *")
MESSAGE("*     . src/scripts/tsp_dev.login.sh    [sh/bash/zsh/...] *")
MESSAGE("*  And then you may build with a simple                   *")
MESSAGE("*     make                                                *")  
MESSAGE("*                                                         *")
MESSAGE("*  Enjoy TSP !!!                                          *")
MESSAGE("*  The TSP Team -- ${PACKAGE_BUGREPORT}                   *")
MESSAGE("***********************************************************")



