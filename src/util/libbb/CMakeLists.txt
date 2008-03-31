OPTION(BUILD_SYSV_BB
  "SysV API for Blackboards" ON)
OPTION(BUILD_POSIX_BB
  "Posix API for Blackboards" OFF)

# FIXME: temporary hack for Rtems
IF(BUILD_RTEMS)
	SET(BUILD_POSIX_BB ON)
	SET(BUILD_SYSV_BB OFF)
ENDIF(BUILD_RTEMS)

IF(BUILD_POSIX_BB)
	SET(POSIX_BB_SRC bb_core_posix.c)
	SET(POSIX_BB_INC bb_core_posix.h)
	ADD_DEFINITIONS(-DUSE_POSIX_BB)
ELSE(BUILD_POSIX_BB)
	SET(POSIX_BB_SRC "")
	SET(POSIX_BB_INC "")
ENDIF(BUILD_POSIX_BB)

IF(BUILD_SYSV_BB)
	SET(SYSV_BB_SRC bb_core_sysv.c)
	SET(SYSV_BB_INC bb_core_sysv.h)
ELSE(BUILD_SYSV_BB)
	SET(SYSV_BB_SRC "")
	SET(SYSV_BB_INC "")
ENDIF(BUILD_SYSV_BB)

SET(_SRC
  bb_alias
  bb_core bb_simple
  bb_sha1 bb_tools 
  bb_utils
  ${SYSV_BB_SRC}
  ${POSIX_BB_SRC})

SET(LIB_VARNAME_SARRAY_SRC bb_varname_sarray.c)

IF(ZLIB_FOUND)
SET(LIB_VARNAME_ZIP_SRC ${_SRC} bb_varname_zip.c)
ENDIF(ZLIB_FOUND)

#
# bb_core_k kernel device API on works on linux
#
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
  SET(_SRC
    ${_SRC}
    bb_core_k.c)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

SET(BB_INCLUDE_FILES
  bb_core.h
  bb_simple.h
  bb_alias.h
  bb_tools.h
  bb_utils.h
  bb_sha1.h
  ${SYSV_BB_INC}
  ${POSIX_BB_INC})

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
  SET(BB_INCLUDE_FILES 
    ${BB_INCLUDE_FILES}
    bb_core_k.h)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

TSP_XINCLUDE(${BB_INCLUDE_FILES})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libbb)

IF (XML2_FOUND)
  SET(_SRC
    ${_SRC}
    bb_xml.c)
  ADD_DEFINITIONS(${XML2_CFLAGS} -DXML2_FOUND)
ENDIF (XML2_FOUND)

IF(ZLIB_FOUND)
ADD_LIBRARY(sarray ${LIB_VARNAME_SARRAY_SRC_SRC})
ENDIF(ZLIB_FOUND)

ADD_LIBRARY(bb ${_SRC})
IF (XML2_FOUND)
  TARGET_LINK_LIBRARIES(bb ${XML2_LIBRARY})
ENDIF (XML2_FOUND)
TSP_LIBRARY_INSTALL(bb)
IF (BUILD_POSIX_BB AND NOT BUILD_RTEMS)
  TARGET_LINK_LIBRARIES(bb rt)
ENDIF (BUILD_POSIX_BB AND NOT BUILD_RTEMS)

IF (NOT BUILD_RTEMS)
  TARGET_LINK_LIBRARIES(bb ${PTHREAD_LIBRARY_NAME} dl)
ENDIF (NOT BUILD_RTEMS)
TSP_LIBRARY_INSTALL(bb)

IF(NOT BUILD_RTEMS)
  ADD_LIBRARY(sarray ${LIB_VARNAME_SARRAY_SRC})
  TSP_LIBRARY_INSTALL(sarray)
ENDIF(NOT BUILD_RTEMS)

IF (NOT BUILD_RTEMS)
  ADD_SUBDIRECTORY(bbtools)
ENDIF (NOT BUILD_RTEMS)

IF(BUILD_SCRIPT_BINDING)
	ADD_SUBDIRECTORY(scripting)
ENDIF(BUILD_SCRIPT_BINDING)

 
# Build a kernel module
# This is a bit tricky: building a kernel module needs a Makefile which
# is not the one provided by cmake. The workaround is in build_module.sh
# which copies Makefile.k to Makefile, does the build and then restores
# the original Makefile.
IF(BUILD_BB_KERNEL_MODULE)
  SET(MODULE_SRC 
    bb_core.c bb_alias.c
    bb_core_k.c bb_module.c
    bb_simple.c)
    
  ADD_CUSTOM_COMMAND(OUTPUT bb.ko
    COMMAND ${CMAKE_SOURCE_DIR}/src/util/libbb/build_module.sh
    DEPENDS ${MODULE_SRC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/util/libbb)
  ADD_CUSTOM_TARGET(kernel_module ALL DEPENDS bb.ko)
ENDIF(BUILD_BB_KERNEL_MODULE)

