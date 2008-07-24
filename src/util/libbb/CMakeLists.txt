INCLUDE(CheckIncludeFiles)

OPTION(BUILD_SYSV_BB
  "SysV API for Blackboards" ON)
OPTION(BUILD_POSIX_BB
  "Posix API for Blackboards" OFF)
OPTION(ALLOW_KBB_WRITE_ACCESS_FROM_USERSPACE
  "Allow write access from userspace to kbb -- Dangerous: Could produce kernel crash" OFF)

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

SET(KERNEL_BUILD_EXTRA_CFLAGS "")
IF (ALLOW_KBB_WRITE_ACCESS_FROM_USERSPACE)
	ADD_DEFINITIONS(-DALLOW_KBB_WRITE_ACCESS_FROM_USERSPACE)
	SET(KERNEL_BUILD_EXTRA_CFLAGS
	  ${KERNEL_BUILD_EXTRA_CFLAGS} " -DALLOW_KBB_WRITE_ACCESS_FROM_USERSPACE")
ELSE (ALLOW_KBB_WRITE_ACCESS_FROM_USERSPACE)
ENDIF (ALLOW_KBB_WRITE_ACCESS_FROM_USERSPACE)

SET(_SRC
  bb_alias.c
  bb_core.c bb_simple.c
  bb_sha1.c bb_tools.c
  bb_utils.c
  bb_local.c
  bb_callback.c
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
  CHECK_INCLUDE_FILES(linux/connector.h CONNECTOR_AVAILABLE CONNECTOR_AVAILABLE)
  IF (CONNECTOR_AVAILABLE)
    ADD_DEFINITIONS(-DCONNECTOR_AVAILABLE)
    SET(_SRC
      ${_SRC}
      bb_core_k.c)
  ELSE (CONNECTOR_AVAILABLE)
    MESSAGE ("Missing linux/connector.h: No kernel blackboards access from userspace")
  ENDIF (CONNECTOR_AVAILABLE)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

SET(BB_INCLUDE_FILES
  bb_core.h
  bb_simple.h
  bb_alias.h
  bb_tools.h
  bb_utils.h
  bb_sha1.h
  bb_local.h
  bb_callback.h
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
# This is a bit tricky: building a kernel module needs a Makefile
# which is not the one provided by cmake.
# The workaround is in build_module.sh which creates a "kbuild"
# directory, copies everythings needed in it and does the build.
SET(MODULE_SRC 
  bb_core.c bb_alias.c
  bb_core_k.c bb_module.c
  bb_simple.c bb_alias.c 
  bb_callback.c )

SET(MODULE_HDR 
  bb_alias.h bb_core.h
  bb_core_k.h bb_simple.h 
  bb_utils.h bb_alias.h
  bb_callback.h)

IF(BUILD_BB_KERNEL_MODULE)
   
  CONFIGURE_FILE(Makefile.k.in ${CMAKE_BINARY_DIR}//src/util/libbb/Makefile.k @ONLY)

  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/src/util/libbb/kbuild/bb.ko
    COMMAND ${CMAKE_SOURCE_DIR}/src/util/libbb/build_module.sh ${CMAKE_SOURCE_DIR}/src/util/libbb ${CMAKE_BINARY_DIR}/src/util/libbb
    DEPENDS Makefile.k.in ${MODULE_SRC} ${MODULE_HDR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/util/libbb)
  ADD_CUSTOM_TARGET(kernel_module ALL DEPENDS ${CMAKE_BINARY_DIR}/src/util/libbb/kbuild/bb.ko)

ENDIF(BUILD_BB_KERNEL_MODULE)


# Build a debian kernel module-assistant package
# This is a bit tricky: building a debian kernel module-assistant
# package needs a special evironment which is not the one provided by
# cmake.
# The workaround is in build_kbb_debian.sh which creates a
# "kbb_debian_build" directory, copies everythings needed in it and
# does the build.
SET(DEBIAN_KBB_BUILD_FILE
  kbb_debian/README.Debian
  kbb_debian/changelog.in
  kbb_debian/compat
  kbb_debian/control
  kbb_debian/control.modules.in
  kbb_debian/copyright
  kbb_debian/kbb-_KVERS_.postinst.modules.in
  kbb_debian/rules)

IF(BUILD_BB_KERNEL_DEBIAN_PACKAGE)

  CONFIGURE_FILE(kbb_debian/changelog.in ${CMAKE_BINARY_DIR}/src/util/libbb/debian_changelog @ONLY)

  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/src/util/libbb/kbb-source_${PACKAGE_VERSION}-1_all.deb
    COMMAND ${CMAKE_SOURCE_DIR}/src/util/libbb/build_kbb_debian.sh ${CMAKE_SOURCE_DIR}/src/util/libbb ${CMAKE_BINARY_DIR}/src/util/libbb
    DEPENDS Makefile.k.in ${MODULE_SRC} ${MODULE_HDR} ${DEBIAN_KBB_BUILD_FILE}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/util/libbb)
  ADD_CUSTOM_TARGET(debian_kbb ALL DEPENDS ${CMAKE_BINARY_DIR}/src/util/libbb/kbb-source_${PACKAGE_VERSION}-1_all.deb)

ENDIF(BUILD_BB_KERNEL_DEBIAN_PACKAGE)
