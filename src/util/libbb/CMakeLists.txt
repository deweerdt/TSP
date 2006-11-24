SET(_SRC
  bb_utils bb_alias
  bb_core bb_simple
  bb_sha1 bb_tools bb_core_sysv.c)

SET(BB_INCLUDE_FILES
  bb_core.h
  bb_simple.h
  bb_alias.h
  bb_tools.h
  bb_utils.h
  bb_sha1.h
  bb_core_sysv.h)

TSP_XINCLUDE(${BB_INCLUDE_FILES})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libbb)

ADD_LIBRARY(bb ${_SRC})
TSP_LIBRARY_INSTALL(bb)
TARGET_LINK_LIBRARIES(bb ${PTHREAD_LIBRARY_NAME})

ADD_SUBDIRECTORY(bbtools)

IF(BUILD_SCRIPT_BINDING)
	ADD_SUBDIRECTORY(scripting)
ENDIF(BUILD_SCRIPT_BINDING)

