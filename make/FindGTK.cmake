MESSAGE(STATUS "Looking for gtk-config...")
FIND_PROGRAM(GTK_CONFIG_PROGRAM 
  NAMES gtk-config 
  PATHS /usr/local/bin /usr/bin
  DOC "The gtk+1.2 configuration tool")
IF (GTK_CONFIG_PROGRAM) 
  MESSAGE(STATUS "Looking for gtk-config - found : ${GTK_CONFIG_PROGRAM}")
  SET(GTK_FOUND "YES")
  SET(GTK_LIBRARY "`${GTK_CONFIG_PROGRAM} --libs`")
  SET(GTK_CFLAGS "`${GTK_CONFIG_PROGRAM} --cflags`")
ELSE (GTK_CONFIG_PROGRAM)
  MESSAGE(STATUS "Looking for gtk-config - NOT found") 
ENDIF (GTK_CONFIG_PROGRAM)