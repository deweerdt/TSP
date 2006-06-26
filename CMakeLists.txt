PROJECT(TSP)

OPTION(BUILD_PROVIDER
  "Build providers" ON)

OPTION(BUILD_CONSUMER
  "Build consumers" ON)

SET(PACKAGE_VERSION "TSP-0.8.0cvs")
INCLUDE(CheckFunctionExists)

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

FIND_FILE(USABLE_GLUT GL/glut.h /usr/include /usr/local/include)

FIND_PROGRAM(LEX_PROGRAM lex /usr/local/bin /usr/bin)
FIND_PROGRAM(YACC_PROGRAM yacc /usr/local/bin /usr/bin)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec/${CMAKE_SYSTEM_NAME}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec/${CMAKE_SYSTEM_NAME}/bin)

LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

FIND_PROGRAM(CMAKE_GTK_CONFIG gtk-config /usr/local/bin /usr/bin)
FIND_PROGRAM(CMAKE_XML_CONFIG xml2-config /usr/local/bin /usr/bin)
SET(XML2_LIBS "`${CMAKE_XML_CONFIG} --libs`")
SET(XML2_FLAGS "`${CMAKE_XML_CONFIG} --cflags`")
SET(GTK_LIBS "`${CMAKE_GTK_CONFIG} --libs`")
SET(GTK_FLAGS "`${CMAKE_GTK_CONFIG} --cflags`")


SUBDIRS(src external)
