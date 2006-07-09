PROJECT(TSP)

OPTION(BUILD_PROVIDER
  "Build providers" ON)

OPTION(BUILD_CONSUMER
  "Build consumers" ON)

OPTION(BUILD_API_DOC
  "Build doxygen documentation" OFF)

SET(PACKAGE_VERSION "tsp-0.8.cvs")

MESSAGE(STATUS "Configuring ${PACKAGE_VERSION}")
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
  ${CMAKE_SOURCE_DIR}/src/core/misc_utils
  ${CMAKE_SOURCE_DIR}/src/util/libpages
  ${CMAKE_SOURCE_DIR}/src/util/libutil
  ${CMAKE_SOURCE_DIR}/src/core/ctrl_init
  ${CMAKE_SOURCE_DIR}/src/core/ctrl
  ${CMAKE_SOURCE_DIR}/src/util/libtspcfg/
  )

#
# Be nice use FIND PACKAGE for searching library
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
MESSAGE(STATUS "Looking for lex...")
FIND_PROGRAM(LEX_PROGRAM 
  NAMES lex flex 
  PATHS /usr/local/bin /usr/bin
  DOC "A lex-compatible lexer generator")
IF (LEX_PROGRAM) 
  MESSAGE(STATUS "Looking for lex... - found lex is ${LEX_PROGRAM}")
ELSE (LEX_PROGRAM) 
  MESSAGE(STATUS "Looking for lex... - NOT found")
ENDIF (LEX_PROGRAM) 

MESSAGE(STATUS "Looking for yacc...")
FIND_PROGRAM(YACC_PROGRAM 
  NAMES yacc bison
  PATHS /usr/local/bin /usr/bin
  DOC "A yacc-compatible parser generator")
IF (YACC_PROGRAM) 
  MESSAGE(STATUS "Looking for yacc... - found yacc is ${YACC_PROGRAM}")
ELSE (YACC_PROGRAM)
  MESSAGE(STATUS "Looking for yacc... - NOT found")
ENDIF (YACC_PROGRAM)


MESSAGE(STATUS "Looking for gtk-config...")
FIND_PROGRAM(GTK_CONFIG_PROGRAM 
  NAMES gtk-config 
  PATHS /usr/local/bin /usr/bin
  DOC "The gtk+1.2 configuration tool")
IF (GTK_CONFIG_PROGRAM) 
  MESSAGE(STATUS "Looking for gtk-config - found : ${GTK_CONFIG_PROGRAM}")
  SET(GTK_FOUND "YES")
  SET(GTK_LIBRARY "`${GTK_CONFIG_PROGRAM} --libs`")
  SET(GTK_CFLAGS "`${GTK_CONFIG_PROGRAM} --cflags`")
ELSE (GTK_CONFIG_PROGRAM)
  MESSAGE(STATUS "Looking for gtk-config - NOT found") 
ENDIF (GTK_CONFIG_PROGRAM)

MESSAGE(STATUS "Looking for xml2-config...")
FIND_PROGRAM(XML2_CONFIG_PROGRAM
  NAMES xml2-config 
  PATHS /usr/local/bin /usr/bin
  DOC "The libxml2 (www.xmlsoft.org) configuration tool")
IF (XML2_CONFIG_PROGRAM)
  MESSAGE(STATUS "Looking for xml2-config - found : ${XML2_CONFIG_PROGRAM}")
  SET(XML2_FOUND "YES")
  SET(XML2_LIBRARY "`${XML2_CONFIG_PROGRAM} --libs`")
  SET(XML2_CFLAGS "`${XML2_CONFIG_PROGRAM} --cflags`")

  EXEC_PROGRAM(${XML2_CONFIG_PROGRAM} 
    ARGS "--version"
    OUTPUT_VARIABLE XML2_VERSION)

  SET(XML2_MINIMAL_VERSION "2.6.0")

  IF (XML2_VERSION STRGREATER XML2_MINIMAL_VERSION)
    MESSAGE(STATUS "Checking libxml2 version ${XML2_VERSION} > ${XML2_MINIMAL_VERSION}, XML Writer API Supported")
    SET(XML2_XMLWRITER_SUPPORTED "YES")
  ELSE (XML2_VERSION STRGREATER XML2_MINIMAL_VERSION)
    MESSAGE(STATUS "Checking libxml2 version ${XML2_VERSION} < ${XML2_MINIMAL_VERSION}, XML Writer API NOT Supported")
    SET(XML2_XMLWRITER_SUPPORTED "NO")
  ENDIF (XML2_VERSION STRGREATER XML2_MINIMAL_VERSION)
ELSE (XML2_CONFIG_PROGRAM)
  SET(XML2_FOUND "NO")
  MESSAGE(STATUS "Looking for xml2-config - NOT found") 
ENDIF (XML2_CONFIG_PROGRAM)

################ FUSE Check ####################
MESSAGE(STATUS "Looking for FUSE headers ...")
FIND_PATH(FUSE_INCLUDE 
  fuse.h 
  /usr/include /usr/local/include
  DOC "The FUSE header")
IF(FUSE_INCLUDE)
	MESSAGE(STATUS "Looking for FUSE headers - found : ${FUSE_INCLUDE}")
ELSE(FUSE_INCLUDE)
	MESSAGE(STATUS "Looking for FUSE headers - NOT found")
ENDIF(FUSE_INCLUDE)

FIND_PATH(XATTR_INCLUDE
  xattr.h 
  /usr/include/attr /usr/local/include/attr
  DOC "The xattr header")

IF(XATTR_INCLUDE)
	MESSAGE(STATUS "Looking for XATTR headers - found : ${XATTR_INCLUDE}")
ELSE(XATTR_INCLUDE)
	MESSAGE(STATUS "Looking for XATTR headers - NOT found")
ENDIF(XATTR_INCLUDE)

MESSAGE(STATUS "Looking for FUSE library ...")
FIND_LIBRARY(FUSE_LIBRARY
  NAMES fuse 
  PATHS /usr/lib /usr/local/lib
  DOC "The FUSE library")

IF(FUSE_LIBRARY)
	MESSAGE(STATUS "Looking for FUSE library - found : ${FUSE_LIBRARY}")
ELSE(FUSE_LIBRARY)
	MESSAGE(STATUS "Looking for FUSE library - NOT found")
ENDIF(FUSE_LIBRARY)

IF(FUSE_INCLUDE AND XATTR_INCLUDE AND FUSE_LIBRARY)
	SET(FUSE_FOUND "YES")
	MESSAGE(STATUS "FUSE consumer enabled")
ELSE(FUSE_INCLUDE AND XATTR_INCLUDE AND FUSE_LIBRARY)
	SET(FUSE_FOUND "NO")
	MESSAGE(STATUS "FUSE consumer disabled")
ENDIF(FUSE_INCLUDE AND XATTR_INCLUDE AND FUSE_LIBRARY)

################ BUILD related config ############################
IF (NOT CMAKE_BUILD_TYPE)
 SET(CMAKE_BUILD_TYPE "Debug")
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/bin)

SET(SCRIPTS_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/scripts)

MAKE_DIRECTORY(${SCRIPTS_OUTPUT_PATH})
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

################ INSTALL related config ############################
SET(CMAKE_INSTALL_PREFIX /opt/${PACKAGE_VERSION})

ADD_CUSTOM_TARGET(cmake_clean
  COMMAND sh -c "rm -rf  ${CMAKE_SOURCE_DIR}/exec"
  COMMAND sh -c "rm -f ${CMAKE_SOURCE_DIR}/CMakeCache.txt"
  COMMAND sh -c "${CMAKE_SOURCE_DIR}/src/scripts/purge_CMakeFiles.sh ${CMAKE_SOURCE_DIR}")

MESSAGE(STATUS "Configuring ${PACKAGE_VERSION}...Done.")

## NOW add subdirectories
ADD_SUBDIRECTORY(external)
ADD_SUBDIRECTORY(src)
