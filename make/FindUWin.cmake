
SET(PATH_DIR "C:/Program Files/UWIN/usr")

MESSAGE(STATUS "Looking for UWin...")

FIND_FILE(UWIN_INCLUDE
	NAMES uwin.h pthread.h
	PATHS ${PATH_DIR}/include
	DOC "The UWin Include Files")
	
FIND_LIBRARY(UWIN_LIBRARY
	NAMES pthread posix
	PATHS ${PATH_DIR}/lib
	DOC "The UWin Library")

###########################################
# TO DO : Find the DLL
###########################################

IF (UWIN_INCLUDE AND UWIN_LIBRARY)
  MESSAGE(STATUS "Looking for UWin... - found UWin lib is ${UWIN_LIBRARY} and include is ${UWIN_INCLUDE}")
  SET(UWIN_FOUND "YES")
  GET_FILENAME_COMPONENT(UWIN_LIBRARY_DIRS ${UWIN_LIBRARY} PATH)
  GET_FILENAME_COMPONENT(UWIN_INCLUDE_DIRS ${UWIN_INCLUDE} PATH)
ELSE (UWIN_INCLUDE AND UWIN_LIBRARY)
  SET(UWIN_FOUND "NO")
  MESSAGE(STATUS "Looking for UWin... - NOT found")
ENDIF (UWIN_INCLUDE AND UWIN_LIBRARY)