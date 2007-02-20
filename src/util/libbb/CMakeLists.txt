SET(_SRC
  bb_utils bb_alias
  bb_core bb_simple
  bb_sha1 bb_tools 
  bb_core_sysv.c)

IF(ZLIB_FOUND)
SET(_SRC ${_SRC} bb_zip.c)
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
  bb_core_sysv.h
  bb_core_k.h)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
  SET(BB_INCLUDE_FILES 
    ${BB_INCLUDE_FILES}
    bb_core_k.h)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

TSP_XINCLUDE(${BB_INCLUDE_FILES})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libbb)

ADD_LIBRARY(bb ${_SRC})
TSP_LIBRARY_INSTALL(bb)
IF(ZLIB_FOUND)
TARGET_LINK_LIBRARIES(bb ${PTHREAD_LIBRARY_NAME} z)
ELSE(ZLIB_FOUND)
TARGET_LINK_LIBRARIES(bb ${PTHREAD_LIBRARY_NAME})
ENDIF(ZLIB_FOUND)

ADD_SUBDIRECTORY(bbtools)

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

