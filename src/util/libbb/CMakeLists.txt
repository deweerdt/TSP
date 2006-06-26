SET(_SRC
bb_utils bb_alias
bb_core bb_simple
bb_sha1 bb_tools)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libbb)

ADD_LIBRARY(bb ${_SRC})
TARGET_LINK_LIBRARIES(bb pthread)

SUBDIRS(bbtools)

IF(BUILD_SCRIPT_BINDING)
	SUBDIRS(scripting)
ENDIF(BUILD_SCRIPT_BINDING)

