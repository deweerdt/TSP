PROJECT(TSP C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)

# May be used to print extensive system informations
#INCLUDE(CMakePrintSystemInformation)

# Use TSP specific CMake modules first
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/make)
# Load TSP Specific CMake macros (TSP_XSCRIPT, TSP_XINCLUDE, TSP_xxx_INSTALL)
INCLUDE(TSPCMakeTools)

# Load Checker macros
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)

#INCLUDE(TestBigEndian)
#TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
#MESSAGE(STATUS "IS_BIG_ENDIAN is : ${IS_BIG_ENDIAN}")

OPTION(BUILD_ONLY_TSP_PORT
  "Build TSP test port" OFF)

OPTION(BUILD_PROVIDER
  "Build providers" ON)

OPTION(BUILD_CONSUMER
  "Build consumers" ON)

OPTION(BUILD_XMLRPC
  "Build with XMLRPC support" OFF)

OPTION(BUILD_API_DOC
  "Build doxygen documentation" OFF)

OPTION(USE_XDR_LIB_ENCODING
  "Use XDR library instead of TSP own XDR encoding"
  OFF)

OPTION(BUILD_BB_KERNEL_MODULE
  "Build the Black Board kernel module" OFF)

IF (WIN32)
  OPTION(BUILD_WIN32_PTHREADWIN32
    "Build under Windows with PthreadWin32" ON)
  
  OPTION(BUILD_WIN32_ACPLT_ONCPRC
    "Build under Windows with ACPLT_ONCPRC" ON)	
  
  OPTION(BUILD_SHARED
    "Build libraries as DLL" ON)
ELSE (WIN32)
    OPTION(BUILD_SHARED
      "Build libraries as shared library" ON)
ENDIF (WIN32)

IF (NOT USE_XDR_LIB_ENCODING)
  ADD_DEFINITIONS(-DTSP_NO_XDR_ENCODE)
ENDIF (NOT USE_XDR_LIB_ENCODING)

IF (BUILD_SHARED)
  SET(BUILD_SHARED_LIBS ON)
  IF (WIN32)
    ADD_DEFINITIONS(-DTSP_SHARED_LIBS)
  ENDIF (WIN32)
ELSE (BUILD_SHARED)
  SET(BUILD_SHARED_LIBS OFF)
ENDIF (BUILD_SHARED)

## If no build type specified then default to Debug
IF (NOT CMAKE_BUILD_TYPE)
 SET(CMAKE_BUILD_TYPE "Debug")
 IF (CMAKE_COMPILER_IS_GNUCC)
    ADD_DEFINITIONS(-Wall)
 ENDIF (CMAKE_COMPILER_IS_GNUCC)
ENDIF (NOT CMAKE_BUILD_TYPE)

# Use this in order to make Eclipse CDT parser handle error message smoothly
# see  http://www.cmake.org/Wiki/CMake:Eclipse
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#Generic is a hack, Platform/Rtems should be submitted to cmake team
IF(CMAKE_SYSTEM_NAME STREQUAL "Rtems")
	SET(BUILD_RTEMS ON)
	SET(BUILD_CONSUMER OFF)
	SET(BUILD_SHARED OFF)
	SET(BUILD_SHARED_LIBS OFF)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Rtems")

SET(PACKAGE_NAME      "tsp")
SET(PACKAGE_VERSION_MAJOR "0")
SET(PACKAGE_VERSION_MINOR "8")
SET(PACKAGE_VERSION_PATCH "4cvs")
SET(PACKAGE_VERSION   "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
SET(PACKAGE_BUGREPORT "tsp@astrium.eads.net")

MESSAGE(STATUS "Configuring ${PACKAGE_NAME} version ${PACKAGE_VERSION}")

################ Visual Studio (VC) Check ####################
IF (WIN32)
  SET(CMAKE_INSTALL_VS "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7")
  SET(TSP_WIN32_LIB TspWin32)
  INCLUDE_DIRECTORIES(external/TspWin32)
ELSE (WIN32)
  SET(TSP_WIN32_LIB)
ENDIF (WIN32)

################ Socket library Check ####################
CHECK_LIBRARY_EXISTS("socket" "connect" "/usr/lib" SOCKET_LIBRARY_HAS_CONNECT)
IF (SOCKET_LIBRARY_HAS_CONNECT)
  MESSAGE(STATUS "Socket library needed for system ${CMAKE_SYSTEM_NAME}")
  SET(SOCKET_LIBRARY "socket")
ELSE (SOCKET_LIBRARY_HAS_CONNECT)
  SET(SOCKET_LIBRARY)
ENDIF (SOCKET_LIBRARY_HAS_CONNECT)

################ PthreadWin32 Check ####################
IF (BUILD_WIN32_PTHREADWIN32)
	FIND_PACKAGE(PthreadWin32 REQUIRED)
	IF (PTHREADWIN32_FOUND)
		LINK_DIRECTORIES(${PTHREADWIN32_LIBRARY_DIRS})
		INCLUDE_DIRECTORIES(${PTHREADWIN32_INCLUDE_DIRS})		
		GET_FILENAME_COMPONENT(PTHREAD_LIBRARY_NAME ${PTHREADWIN32_LIBRARY} NAME_WE)
	ENDIF (PTHREADWIN32_FOUND)
ENDIF (BUILD_WIN32_PTHREADWIN32)

################ ACPLT_ONCRPC Check ####################
IF (BUILD_WIN32_ACPLT_ONCPRC)
	FIND_PACKAGE(ACPLT_ONCRPC)
	IF (ACPLT_ONCRPC_FOUND)
		LINK_DIRECTORIES(${ACPLT_ONCRPC_LIBRARY_DIRS})
		INCLUDE_DIRECTORIES(${ACPLT_ONCRPC_INCLUDE_DIRS})
	ENDIF (ACPLT_ONCRPC_FOUND)
	FIND_PACKAGE(ONCRPC)
ELSE (BUILD_WIN32_ACPLT_ONCPRC)
  FIND_PACKAGE(ONCRPC)
ENDIF (BUILD_WIN32_ACPLT_ONCPRC)

IF (WIN32)	
	ADD_DEFINITIONS(-DTSP_SYSTEM_USE_WIN32_SLEEP)
ELSE (WIN32)
	CHECK_FUNCTION_EXISTS(nanosleep HAS_NANOSLEEP)
	IF(HAS_NANOSLEEP)
		ADD_DEFINITIONS(-DTSP_SYSTEM_HAVE_NANOSLEEP)
	ELSE (HAS_NANOSLEEP)
		CHECK_FUNCTION_EXISTS(usleep HAS_THREADSAFE_USLEEP)
		IF (HAS_THREADSAFE_USLEEP)
			ADD_DEFINITIONS(-DTSP_SYSTEM_HAVE_THREADSAFE_USLEEP)
		ENDIF (HAS_THREADSAFE_USLEEP)
	ENDIF(HAS_NANOSLEEP)
			
	CHECK_FUNCTION_EXISTS(gethrtime HAS_GETHRTIME)
	IF(HAS_GETHRTIME)
		ADD_DEFINITIONS(-DTSP_SYSTEM_HAVE_GETHRTIME)
	ENDIF (HAS_GETHRTIME)
	SET(PTHREAD_LIBRARY_NAME "pthread")
	ADD_DEFINITIONS(-D_REENTRANT)
ENDIF (WIN32)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}/src/core/include
  ${CMAKE_BINARY_DIR}/src/core
  ${CMAKE_BINARY_DIR}/src/core/rpc
  ${CMAKE_SOURCE_DIR}/src/core/include
  ${CMAKE_SOURCE_DIR}/src/core/
  ${CMAKE_SOURCE_DIR}/src/core/driver
  ${CMAKE_SOURCE_DIR}/src/core/common
  ${CMAKE_SOURCE_DIR}/src/core/misc_utils
  ${CMAKE_SOURCE_DIR}/src/util/libpages
  ${CMAKE_SOURCE_DIR}/src/util/libutil
  ${CMAKE_SOURCE_DIR}/src/core/ctrl_init
  ${CMAKE_SOURCE_DIR}/src/core/ctrl
  ${CMAKE_SOURCE_DIR}/src/util/libtspcfg
  )


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/core/rpc)  
IF (BUILD_XMLRPC)
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/core/xmlrpc)
ENDIF (BUILD_XMLRPC)
  


#
# Be nice use FIND PACKAGE for searching library
# if there is no predefined CMake FindXXX module
# for your needs please write one
# and put it in {TSP_SRC_BASE}/make directory
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
  SET(GLUT_DEVEL_PACKAGE "glut-devel")
ELSE (GLUT_FOUND)
  MESSAGE(STATUS "Looking for GLUT - not found")
ENDIF (GLUT_FOUND)

################ LEX/YACC Check ####################
FIND_PACKAGE(LexYacc)
################ GTK+1 Check ####################
FIND_PACKAGE(GTK)
################ XML2 Check ####################
FIND_PACKAGE(XML2)
IF (XML2_XMLWRITER_SUPPORTED)
  ADD_DEFINITIONS(-DXMLWRITER_SUPPORTED)
ENDIF (XML2_XMLWRITER_SUPPORTED)

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

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
  ADD_DEFINITIONS(-DUSE_STRACE)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

IF (WIN32)
  SET(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/bin)
  SET(EXECUTABLE_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH})
  IF (BUILD_WIN32_PTHREADWIN32 AND PTHREADWIN32_FOUND)
	## installer
	INSTALL(PROGRAMS ${PTHREADWIN32_INSTALL} DESTINATION external/${PTHREAD_LIBRARY_NAME}/sources)
	## external include
	INSTALL(FILES	${PTHREADWIN32_INCLUDE_DIRS}/pthread.h 
					${PTHREADWIN32_INCLUDE_DIRS}/sched.h 
					${PTHREADWIN32_INCLUDE_DIRS}/semaphore.h 
			DESTINATION external/${PTHREAD_LIBRARY_NAME}/include)
	## external lib and dll
	INSTALL(FILES ${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.lib 
			DESTINATION external/${PTHREAD_LIBRARY_NAME}/lib)
	INSTALL(FILES ${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.dll 
			DESTINATION external/${PTHREAD_LIBRARY_NAME}/bin)
    ## Copy .dll ##    
    CONFIGURE_FILE(${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.dll ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/${PTHREAD_LIBRARY_NAME}.dll COPYONLY)
	## Install ##
	INSTALL(FILES ${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.dll DESTINATION bin)
    
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
		## debug file ##
		IF (EXISTS ${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.pdb)
		   CONFIGURE_FILE(${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.pdb ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/${PTHREAD_LIBRARY_NAME}.pdb COPYONLY)
		   INSTALL(FILES ${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.pdb DESTINATION bin)
		   INSTALL(FILES ${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.pdb DESTINATION external/${PTHREAD_LIBRARY_NAME}/bin)
		ENDIF (EXISTS ${PTHREADWIN32_LIBRARY_DIRS}/${PTHREAD_LIBRARY_NAME}.pdb)
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
  ENDIF (BUILD_WIN32_PTHREADWIN32 AND PTHREADWIN32_FOUND)
  IF (BUILD_WIN32_ACPLT_ONCPRC AND ACPLT_ONCRPC_FOUND)
	## installer
	INSTALL(PROGRAMS ${ACPLT_ONCRPC_INSTALL} DESTINATION external/oncrpc/source)
	## external include	
	INSTALL(FILES	${ACPLT_INCLUDE_DIR1}/AUTH.H
					${ACPLT_INCLUDE_DIR1}/AUTH_UNI.H
					${ACPLT_INCLUDE_DIR1}/BCOPY.H
					${ACPLT_INCLUDE_DIR1}/CLNT.H
					${ACPLT_INCLUDE_DIR1}/NETDB.H
					${ACPLT_INCLUDE_DIR1}/RPC.H
					${ACPLT_INCLUDE_DIR1}/RPC_MSG.H
					${ACPLT_INCLUDE_DIR1}/SVC.H
					${ACPLT_INCLUDE_DIR1}/SVC_AUTH.H
					${ACPLT_INCLUDE_DIR1}/TYPES.H
					${ACPLT_INCLUDE_DIR1}/XDR.H 
			DESTINATION external/oncrpc/include/rpc)
	## external lib and dll
	INSTALL(FILES ${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.lib 
			DESTINATION external/oncrpc/lib)
	INSTALL(FILES ${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.dll 
			DESTINATION external/oncrpc/bin)
	# external programs
	INSTALL(PROGRAMS ${ACPLT_ONCRPC_BINARY_DIRS}/rpcinfo.exe
					 ${ACPLT_ONCRPC_BINARY_DIRS}/rpcgen.exe
					 ${ACPLT_ONCRPC_BINARY_DIRS}/portmap.exe
					 ${ACPLT_ONCRPC_BINARY_DIRS}/pm_ascii.exe
					 ${ACPLT_ONCRPC_BINARY_DIRS}/inst_pm.exe
			DESTINATION external/oncrpc/bin)
	## external services
	INSTALL(FILES ${ACPLT_ONCRPC_BINARY_DIRS}/install.bat
			DESTINATION external/oncrpc/service)
	INSTALL(FILES ${ACPLT_ONCRPC_ETC_DIR}/rpc
	        DESTINATION external/oncrpc/etc)
	## Copy .dll ##
    CONFIGURE_FILE(${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.dll ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/oncrpc.dll COPYONLY)
    ## Install ##
    INSTALL(FILES ${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.dll DESTINATION bin)
    
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
		## debug file ##
		IF (EXISTS ${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.pdb)
		   CONFIGURE_FILE(${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.pdb ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/oncrpc.pdb COPYONLY)
		   INSTALL(FILES ${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.pdb DESTINATION bin)
		   INSTALL(FILES ${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.pdb DESTINATION external/oncrpc/bin)
		ENDIF (EXISTS ${ACPLT_ONCRPC_LIBRARY_DIRS}/oncrpc.pdb)
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
  ENDIF (BUILD_WIN32_ACPLT_ONCPRC AND ACPLT_ONCRPC_FOUND)
  
  IF (MSVC71)
    ## Install FIX the path to le system directoy##
    INSTALL(FILES 
      C:/Windows/system32/msvcr71d.dll
      C:/Windows/system32/mfc71d.dll
      DESTINATION external/msvc)
  ENDIF (MSVC71)
ELSE (WIN32)
  #Nothing to do.
  SET(LIBRARY_OUTPUT_PATH    ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/lib)
  SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/bin)
ENDIF (WIN32)

SET(DIST_DIR ${PROJECT_SOURCE_DIR}/dist)
MARK_AS_ADVANCED(DIST_DIR)

LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

################ INSTALL related config ############################
# the CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT is
# CMake 2.4.4 (or later) feature
# see
# http://www.cmake.org/pipermail/cmake/2006-October/011559.html
IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)
ENDIF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

IF (!PKGCONFIG_INSTALL_PREFIX)
   SET(PKGCONFIG_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif (!PKGCONFIG_INSTALL_PREFIX)

################ TSP SDK related config ###########################
TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp_dev.login.in
  ${CMAKE_BINARY_DIR}/src/scripts/tsp_dev.login
  @ONLY)

TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp_dev.login.sh.in 
  ${CMAKE_BINARY_DIR}/src/scripts/tsp_dev.login.sh
  @ONLY)

TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp.pc.in
  ${CMAKE_BINARY_DIR}/src/scripts/tsp.pc
  @ONLY)

TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp_profile.sh.in
  ${CMAKE_BINARY_DIR}/src/scripts/tsp.profile.sh
  @ONLY)

TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  src/scripts/tsp_profile.csh.in
  ${CMAKE_BINARY_DIR}/src/scripts/tsp.profile.csh
  @ONLY)

INSTALL(
  FILES 
  ${CMAKE_BINARY_DIR}/src/scripts/tsp.profile.sh
  ${CMAKE_BINARY_DIR}/src/scripts/tsp.profile.csh
  DESTINATION ${TSP_SHARE_BASEDIR}/scripts)

INSTALL(
  FILES
  ${CMAKE_BINARY_DIR}/src/scripts/tsp.pc
  DESTINATION lib/pkgconfig)

## Now add subdirectories
IF (BUILD_ONLY_TSP_PORT)
  ADD_SUBDIRECTORY(tests/tsp_port)
ELSE (BUILD_ONLY_TSP_PORT)
  ADD_SUBDIRECTORY(tests/tsp_port)
  ADD_SUBDIRECTORY(external)
  ADD_SUBDIRECTORY(src)
ENDIF (BUILD_ONLY_TSP_PORT)

ENABLE_TESTING()

## Patch generating target
# The custom target is using a cmake script which in turn
# use execute_process, 
# this way we may find a flexible way to build the patch
ADD_CUSTOM_TARGET(patch 
                  COMMAND ${CMAKE_COMMAND} -DWDIR:PATH=${CMAKE_SOURCE_DIR} -DODIR:PATH=${CMAKE_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/make/CreatePatchCVS.cmake                                                      
                  COMMENT "Generating patch file ready for review at tsp-devel@nongnu.org")

## Packaging related stuff

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Transport Sample Protocol")
SET(CPACK_PACKAGE_VENDOR "The TSP Team")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_NAME tsp)
SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})

IF (NOT WIN32)
  SET(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
  SET(CPACK_GENERATOR "TGZ")
  EXECUTE_PROCESS(COMMAND uname -m
    OUTPUT_VARIABLE MACHINE)
  STRING(REPLACE "\n" "" 
    MACHINE
    ${MACHINE})
  SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${MACHINE})
ELSE (NOT WIN32) 
  SET(CPACK_SOURCE_GENERATOR "ZIP")
ENDIF (NOT WIN32)


#How to exclude some files from source?
#SET(CPACK_SOURCE_STRIP_FILES "cscope.out;cscope.files")
SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*")
SET(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})

IF(WIN32)
  # FIXME should check if it not redundant with 
  # "manual" NSIS MSVC install
  # advice seen on CMake ML 
  # http://www.cmake.org/pipermail/cmake/2007-May/014048.html
  #SET(CMAKE_INSTALL_MFC_LIBRARIES YES)
  #INCLUDE(InstallRequiredSystemLibraries)
  #�see https://savannah.nongnu.org/task/index.php?6883
  
    ## Customize NSIS installer
    SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/make/\\\\pieuvre_tsp_graph.bmp")
    SET(CPACK_NSIS_CREATE_ICONS "
		CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\TSP Help.lnk\\\" \\\"$INSTDIR\\\\doc\\\\html\\\\tsp.chm\\\"
		CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\TSP RPC CleanUp.lnk\\\" \\\"$INSTDIR\\\\bin\\\\tsp_rpc_cleanup.exe\\\"
	    CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\TSP StubServer.lnk\\\" \\\"$INSTDIR\\\\bin\\\\tsp_stub_server.exe\\\"
	    CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\TSP StdOutClient.lnk\\\" \\\"$INSTDIR\\\\bin\\\\tsp_stdout_client.exe\\\" \\\"-p 10 -s 2 -n 0\\\"
	")
	SET(CPACK_NSIS_DELETE_ICONS "
		Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\TSP Help.lnk\\\"
		Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\TSP RPC CleanUp.lnk\\\"
	    Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\TSP StubServer.lnk\\\"
	    Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\TSP StdOutClient.lnk\\\"
	")
    SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Transport Sample Protocol (TSP)")
    SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\savannah.nongnu.org\\\\projects\\\\tsp")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.ts2p.org\\\\tsp")
    SET(CPACK_NSIS_CONTACT "tsp@astrium.eads.net")
    SET(CPACK_NSIS_MODIFY_PATH ON)
	## Copy dll to the system path	
	IF (BUILD_WIN32_ACPLT_ONCPRC AND ACPLT_ONCRPC_FOUND)
		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
		    ExecWait  \\\"$INSTDIR\\\\external\\\\oncrpc\\\\bin\\\\inst_pm.exe remove\\\"
		    CopyFiles \\\"$INSTDIR\\\\external\\\\oncrpc\\\\bin\\\\oncrpc.dll\\\" \\\"$SYSDIR\\\"
		    CopyFiles \\\"$INSTDIR\\\\external\\\\oncrpc\\\\bin\\\\portmap.exe\\\" \\\"$SYSDIR\\\"
		    CreateDirectory \\\"$SYSDIR\\\\drivers\\\\etc\\\"
		    CopyFiles \\\"$INSTDIR\\\\external\\\\oncrpc\\\\etc\\\\rpc\\\" \\\"$SYSDIR\\\\drivers\\\\etc\\\"
		    ExecWait  \\\"$INSTDIR\\\\external\\\\oncrpc\\\\bin\\\\inst_pm.exe $SYSDIR\\\\portmap.exe\\\"			
		")
	ENDIF (BUILD_WIN32_ACPLT_ONCPRC AND ACPLT_ONCRPC_FOUND)
	
	IF (BUILD_WIN32_PTHREADWIN32 AND PTHREADWIN32_FOUND)
		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
			CopyFiles \\\"$INSTDIR\\\\bin\\\\${PTHREAD_LIBRARY_NAME}.dll\\\" \\\"$SYSDIR\\\"
		")
	ENDIF (BUILD_WIN32_PTHREADWIN32 AND PTHREADWIN32_FOUND)
	
	SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
		CopyFiles \\\"$INSTDIR\\\\bin\\\\TspWin32.dll\\\" \\\"$SYSDIR\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\Tsp_*.dll\\\" \\\"$SYSDIR\\\"
	")
	
	IF (MSVC71)
		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
			IfFileExists \\\"$SYSDIR\\\\msvcr71d.dll\\\" next1 0
			CopyFiles \\\"$INSTDIR\\\\external\\\\msvc\\\\msvcr71d.dll\\\" \\\"$SYSDIR\\\"
			next1:
			IfFileExists \\\"$SYSDIR\\\\mfc71d.dll\\\" next2 0
			CopyFiles \\\"$INSTDIR\\\\external\\\\msvc\\\\mfc71d.dll\\\" \\\"$SYSDIR\\\"
			next2:
		")
	ENDIF (MSVC71)
	
#	SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
#		MessageBox MB_OK \\\"Copy Libraries to the system directory\\\"
#	")
	
	IF (BUILD_WIN32_PTHREADWIN32 AND PTHREADWIN32_FOUND)
		SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
			Delete \\\"$SYSDIR\\\\${PTHREAD_LIBRARY_NAME}.dll\\\"
		")
	ENDIF (BUILD_WIN32_PTHREADWIN32 AND PTHREADWIN32_FOUND)
	
	SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS} "
		Delete \\\"$SYSDIR\\\\TspWin32.dll\\\"
		Delete \\\"$SYSDIR\\\\Tsp_*.dll\\\"
		ExecWait  \\\"$INSTDIR\\\\external\\\\oncrpc\\\\bin\\\\inst_pm.exe remove\\\"
		Delete    \\\"$SYSDIR\\\\oncrpc.dll\\\"
		Delete    \\\"$SYSDIR\\\\portmap.exe\\\"
		Delete    \\\"$SYSDIR\\\\drivers\\\\etc\\\\rpc\\\"
	")
	
#	SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS} "
#		MessageBox MB_OK \\\"Delete Libraries from the system directory\\\"
#	")
ENDIF(WIN32)

INCLUDE(CPack)
INCLUDE(UsePackageBackup)

INCLUDE(UseDebian)
IF (DEBIAN_FOUND)
  ADD_DEBIAN_TARGETS(tsp)
ENDIF(DEBIAN_FOUND)

# Try to find Doxygen even if we do not
# build API_DOC for this current build
# since it will be built for RPM
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_PACKAGE AND DOXYGEN_DOT_PACKAGE)
   SET(RPM_MAKE_APIDOC 1)
ELSE (DOXYGEN_PACKAGE AND DOXYGEN_DOT_PACKAGE)
   SET(RPM_MAKE_APIDOC 0)
ENDIF (DOXYGEN_PACKAGE AND DOXYGEN_DOT_PACKAGE)
MARK_AS_ADVANCED(RPM_MAKE_APIDOC)

 
INCLUDE(UseRPMTools)
IF(RPMTools_FOUND)
  RPMTools_ADD_RPM_TARGETS(tsp ${CMAKE_SOURCE_DIR}/src/scripts/tsp.spec.in)
  #RPMTools_ADD_RPM_TARGETS(tsp)
ENDIF(RPMTools_FOUND)

MESSAGE(STATUS "Configuring ${PACKAGE_NAME} version ${PACKAGE_VERSION}...Done.")

IF (WIN32)
  MESSAGE(STATUS "*** TSP for WIN32 has been successfully configured ********")
ELSE (WIN32)
  MESSAGE(STATUS "*** TSP has been successfully configured ********")
ENDIF (WIN32)
