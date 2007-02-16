#
# - Find tools needed for building RPM Packages
#   on Linux systems and defines macro that helps to
#   build source or binary RPM
#
# - Define RPMTools_ADD_RPM_TARGETS which defines
#   two (top-level) CUSTOM targets for building
#   source and binary RPMs
#
# Those CMake macros are provided by the TSP Developer Team
# https://savannah.nongnu.org/projects/tsp
#

IF (WIN32)  
  MESSAGE(STATUS "RPM tools not available on Win32 systems")
ENDIF(WIN32)

IF (UNIX)
  # Look for RPM builder executable
  FIND_PROGRAM(RPMTools_RPMBUILD_EXECUTABLE 
    NAMES rpmbuild
    PATHS "/usr/bin;/usr/lib/rpm"
    PATH_SUFFIXES bin
    DOC "The RPM builder tool")
  
  IF (RPMTools_RPMBUILD_EXECUTABLE)
    MESSAGE(STATUS "Looking for RPMTools... - found rpmuild is ${RPMTools_RPMBUILD_EXECUTABLE}")
    SET(RPMTools_RPMBUILD_FOUND "YES")
    GET_FILENAME_COMPONENT(ONCRPC_BINARY_DIRS ${RPMTools_RPMBUILD_EXECUTABLE} PATH)
  ELSE (RPMTools_RPMBUILD_EXECUTABLE) 
    SET(RPMTools_RPMBUILD_FOUND "NO")
    MESSAGE(STATUS "Looking for ONCRPC... - rpmbuild NOT FOUND")
  ENDIF (RPMTools_RPMBUILD_EXECUTABLE) 
  
  # Detect if CPack was included or not
  IF (NOT DEFINED "CPACK_PACKAGE_NAME") 
    MESSAGE(FATAL_ERROR "CPack was not included, you should include CPack before Using RPMTools")
  ENDIF (NOT DEFINED "CPACK_PACKAGE_NAME")
  
  IF (RPMTools_RPMBUILD_FOUND)
    SET(RPMTools_FOUND TRUE)    
    #
    # - first arg is the RPM spec file
    # - second arg may be RPM ROOT DIRECTORY used to build RPMs (optional)
    #
    MACRO(RPMTools_ADD_RPM_TARGETS RPMNAME SPECFILE_PATH)

      #
      # We may check here wether if we should
      # CONFIGURE_FILE or not (FIXME)
      # 
      # Check provided SPEC file
      GET_FILENAME_COMPONENT(SPECFILE_NAME ${SPECFILE_PATH} NAME)
      
      # Verify whether if RPM_ROOTDIR was provided or not
      IF("${ARGV2}" STREQUAL "") 
	SET(RPM_ROOTDIR ${CMAKE_BINARY_DIR}/RPM)
      ELSE ("${ARGV2}" STREQUAL "")
	SET(RPM_ROOTDIR "${ARGV2}")	
      ENDIF("${ARGV2}" STREQUAL "")
      MESSAGE(STATUS "RPMTools:: Using RPM_ROOTDIR=${RPM_ROOTDIR}")
      
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR})
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/tmp)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/BUILD)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/RPMS)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/SOURCES)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/SPECS)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/SRPMS)
      
      ADD_CUSTOM_TARGET(${RPMNAME}_srpm
	COMMAND ${CMAKE_MAKE_PROGRAM} package_source
	COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz ${RPM_ROOTDIR}/SOURCES    
	COMMAND ${CMAKE_COMMAND} -E copy ${SPECFILE_PATH} ${RPM_ROOTDIR}/SPECS
	COMMAND ${RPMTools_RPMBUILD_EXECUTABLE} -bs --define=\"_topdir ${RPM_ROOTDIR}\" --buildroot=${RPM_ROOTDIR}/tmp ${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME} 
	)
      
      ADD_CUSTOM_TARGET(${RPMNAME}_rpm
	COMMAND ${CMAKE_MAKE_PROGRAM} package_source
	COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz ${RPM_ROOTDIR}/SOURCES    
	COMMAND ${CMAKE_COMMAND} -E copy ${SPECFILE_PATH} ${RPM_ROOTDIR}/SPECS
	COMMAND ${RPMTools_RPMBUILD_EXECUTABLE} -bb --define=\"_topdir ${RPM_ROOTDIR}\" --buildroot=${RPM_ROOTDIR}/tmp ${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME} 
	)  
    ENDMACRO(RPMTools_ADD_RPM_TARGETS)

  ELSE (RPMTools_RPMBUILD_FOUND)
    SET(RPMTools FALSE)
  ENDIF (RPMTools_RPMBUILD_FOUND)  
  
ENDIF (UNIX)
  
