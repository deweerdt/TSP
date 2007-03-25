# - Find xmlrpc-c http://xmlrpc-c.sourceforge.net/
# This module finds if xmlrpc-c is installed and determines where the include files
# and libraries are. It also determines what the name of the library is. This
# code sets the following variables:
#  XMLRPCC_CFLAGS = cflags, (actually -I flag, where xmlrpc-c headers can be found)
#  XMLRPCC_ABYSS_SERVER_LIBRARIES    = the library to link in for use in xmlrpc Abyss server mode
#  XMLRPCC_CLIENT_LIBRARIES    = the library to link in for use in xmlrpc client mode
#  XMLRPCC_VERSION = xmlrpc-c version               

MESSAGE(STATUS "Looking for xmlrpc-c-config...")
FIND_PROGRAM(XMLRPCC_CONFIG_PROGRAM
  NAMES  xmlrpc-c-config
  PATHS /usr/local/bin /usr/bin /opt/xmlrpc /opt/xmlrpc-c
  DOC "The xmlrpc-c (xmlrpc-c.sourceforge.net) configuration tool")
IF (XMLRPCC_CONFIG_PROGRAM)          
    EXEC_PROGRAM(${XMLRPCC_CONFIG_PROGRAM} 
        ARGS "--version"
        OUTPUT_VARIABLE XMLRPCC_VERSION)
    MESSAGE(STATUS "Looking for xmlrpc-c-config - found version ${XMLRPCC_VERSION} : ${XMLRPCC_CONFIG_PROGRAM}")

    SET(XMLRPCC_FOUND "YES")
    EXEC_PROGRAM(${XMLRPCC_CONFIG_PROGRAM} ARGS "--prefix" OUTPUT_VARIABLE XMLRPCC_PREFIX)
    EXEC_PROGRAM(${XMLRPCC_CONFIG_PROGRAM} ARGS "--exec-prefix" OUTPUT_VARIABLE XMLRPCC_EXEC_PREFIX)

    #include
    EXEC_PROGRAM(${XMLRPCC_CONFIG_PROGRAM} ARGS "--cflags" OUTPUT_VARIABLE XMLRPCC_CFLAGS)
    STRING(REPLACE prefix XMLRPCC_PREFIX XMLRPCC_CFLAGS ${XMLRPCC_CFLAGS})
    STRING(CONFIGURE ${XMLRPCC_CFLAGS} XMLRPCC_CFLAGS)

    #abyss server libs 
    EXEC_PROGRAM(${XMLRPCC_CONFIG_PROGRAM} ARGS "abyss-server --libs" OUTPUT_VARIABLE XMLRPCC_ABYSS_SERVER_LIBRARIES)
    STRING(REPLACE exec_prefix XMLRPCC_EXEC_PREFIX XMLRPCC_ABYSS_SERVER_LIBRARIES ${XMLRPCC_ABYSS_SERVER_LIBRARIES})
    STRING(CONFIGURE ${XMLRPCC_ABYSS_SERVER_LIBRARIES} XMLRPCC_ABYSS_SERVER_LIBRARIES)

    #abyss client libs 
    EXEC_PROGRAM(${XMLRPCC_CONFIG_PROGRAM} ARGS "client --libs" OUTPUT_VARIABLE XMLRPCC_CLIENT_LIBRARIES)
    STRING(REPLACE exec_prefix XMLRPCC_EXEC_PREFIX XMLRPCC_CLIENT_LIBRARIES ${XMLRPCC_CLIENT_LIBRARIES})
    STRING(CONFIGURE ${XMLRPCC_CLIENT_LIBRARIES} XMLRPCC_CLIENT_LIBRARIES)

           
ELSE (XMLRPCC_CONFIG_PROGRAM)
  SET(XMLRPCC_FOUND "NO")
  MESSAGE(STATUS "Looking for xmlrpc-c-config - NOT found")     
  IF (XMLRPCC_FIND_REQUIRED) 
    MESSAGE(FATAL_ERROR "xmlrpc-c is required. Aborting.")     
  ENDIF (XMLRPCC_FIND_REQUIRED)
ENDIF (XMLRPCC_CONFIG_PROGRAM)

MARK_AS_ADVANCED(
  XMLRPCC_VERSION
  XMLRPCC_CFLAGS
  XMLRPCC_ABYSS_SERVER_LIBRARIES
  XMLRPCC_CLIENT_LIBRARIES  
  XMLRPCC_PREFIX
  XMLRPCC_EXEC_PREFIX
  XMLRPCC_CONFIG_PROGRAM
  )
