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
  bb_core_sysv.h
  bb_core_k.h)

TSP_XINCLUDE(${BB_INCLUDE_FILES})

SET(MODULE_SRC 
  bb_core.c bb_alias.c
  bb_core_k.c bb_module.c
  bb_simple.c)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libbb)

ADD_LIBRARY(bb ${_SRC})
TSP_LIBRARY_INSTALL(bb)
TARGET_LINK_LIBRARIES(bb ${PTHREAD_LIBRARY_NAME})

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
	ADD_CUSTOM_COMMAND(OUTPUT bb.ko
		COMMAND ${CMAKE_SOURCE_DIR}/src/util/libbb/build_module.sh
		DEPENDS ${MODULE_SRC}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/util/libbb)
	ADD_CUSTOM_TARGET(kernel_module ALL DEPENDS bb.ko)
ENDIF(BUILD_BB_KERNEL_MODULE)

