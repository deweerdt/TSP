# - Find libwww http://www.w3.org/Library/
# This module finds if Libwww is installed and determines where the include files
# and libraries are. It also determines what the name of the library is. This
# code sets the following variables:
#  LIBWWW_CFLAGS = cflags, (actually -I flag, where libwww headers can be found)
#  LIBWWW_LIBRARIES    = the libraries to link in

MESSAGE(STATUS "Looking for libwww...")
FIND_PROGRAM(LIBWWW_CONFIG_PROGRAM
  NAMES  libwww-config
  PATHS /usr/local/bin /usr/bin
  DOC "The libwww (www.w3.org) configuration tool")
IF (LIBWWW_CONFIG_PROGRAM)
          
    SET(LIBWWW_FOUND "YES")
    MESSAGE(STATUS "Looking for libwww-config - found : ${LIBWWW_CONFIG_PROGRAM}")

    EXEC_PROGRAM(${LIBWWW_CONFIG_PROGRAM} ARGS "--cflags" OUTPUT_VARIABLE LIBWWW_CFLAGS)
    EXEC_PROGRAM(${LIBWWW_CONFIG_PROGRAM} ARGS "--libs" OUTPUT_VARIABLE LIBWWW_LIBRARIES)
           
ELSE (LIBWWW_CONFIG_PROGRAM)
  SET(LIBWWW_FOUND "NO")
  MESSAGE(STATUS "Looking for libwww-config - NOT found")     
  IF (LIBWWW_FIND_REQUIRED) 
    MESSAGE(FATAL_ERROR "libwww is required. Aborting.")     
  ENDIF (LIBWWW_FIND_REQUIRED)
ENDIF (LIBWWW_CONFIG_PROGRAM)

MARK_AS_ADVANCED(
  LIBWWW_CFLAGS
  LIBWWW_LIBRARIES
  LIBWWW_CONFIG_PROGRAM
  )
