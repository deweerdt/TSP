# - Find FUSE http://fuse.sourceforge.net/
# This module looks for FUSE support on linux platform
# it will define the following values
#  MPI_INCLUDE_PATH = where mpi.h can be found
#  MPI_LIBRARY    = the library to link in (mpi mpich etc)

IF (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  MESSAGE(STATUS "FUSE not available on non-Linux platform")
  SET(FUSE_FOUND "NO")
ELSE (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  MESSAGE(STATUS "Looking for FUSE headers ...")
  FIND_PATH(FUSE_INCLUDE_DIR 
    fuse.h 
    /usr/include /usr/local/include
    DOC "The FUSE header")
  IF(FUSE_INCLUDE_DIR)
    MESSAGE(STATUS "Looking for FUSE headers - found : ${FUSE_INCLUDE_DIR}")
  ELSE(FUSE_INCLUDE_DIR)
    MESSAGE(STATUS "Looking for FUSE headers - NOT found")
  ENDIF(FUSE_INCLUDE_DIR)
  
  MESSAGE(STATUS "Looking for XATTR headers ...")
  FIND_PATH(XATTR_INCLUDE_DIR
    xattr.h 
    /usr/include/attr /usr/local/include/attr
    DOC "The xattr header")
  IF(XATTR_INCLUDE_DIR)
    MESSAGE(STATUS "Looking for XATTR headers - found : ${XATTR_INCLUDE_DIR}")
  ELSE(XATTR_INCLUDE_DIR)
    MESSAGE(STATUS "Looking for XATTR headers - NOT found")
  ENDIF(XATTR_INCLUDE_DIR)

  MESSAGE(STATUS "Looking for FUSE library ...")
  FIND_LIBRARY(FUSE_LIBRARY
    NAMES fuse 
    PATHS /usr/lib /usr/local/lib
    DOC "The FUSE library")
  
  IF(FUSE_LIBRARY)
    MESSAGE(STATUS "Looking for FUSE library - found : ${FUSE_LIBRARY}")
  ELSE(FUSE_LIBRARY)
    MESSAGE(STATUS "Looking for FUSE library - NOT found")
  ENDIF(FUSE_LIBRARY)

  IF(FUSE_INCLUDE_DIR AND XATTR_INCLUDE_DIR AND FUSE_LIBRARY)
    SET(FUSE_FOUND "YES")    
    SET(FUSE_DEVEL_PACKAGE fuse-devel)
  ELSE(FUSE_INCLUDE_DIR AND XATTR_INCLUDE_DIR AND FUSE_LIBRARY)
    SET(FUSE_FOUND "NO")
    SET(FUSE_DEVEL_PACKAGE "")
  ENDIF(FUSE_INCLUDE_DIR AND XATTR_INCLUDE_DIR AND FUSE_LIBRARY)
ENDIF (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")

MARK_AS_ADVANCED(
  FUSE_INCLUDE_DIR
  XATTR_INCLUDE_DIR
  FUSE_LIBRARY
  )
