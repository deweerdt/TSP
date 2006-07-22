SET(_SRC
bb_utils bb_alias
bb_core bb_simple
bb_sha1 bb_tools bb_core_sysv.c)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libbb)

ADD_LIBRARY(bb ${_SRC})
TARGET_LINK_LIBRARIES(bb pthread)

ADD_SUBDIRECTORY(bbtools)

IF(BUILD_SCRIPT_BINDING)
	ADD_SUBDIRECTORY(scripting)
ENDIF(BUILD_SCRIPT_BINDING)

