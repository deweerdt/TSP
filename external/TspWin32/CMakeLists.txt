ADD_LIBRARY(TspWin32
			getopt 
			basename)

TSP_LIBRARY_INSTALL(TspWin32) 

## external include
INSTALL(FILES	getopt.h
				basename.h
				TspWin32.h
		DESTINATION external/TspWin32/include)
## external lib and dll

INSTALL(
    TARGETS TspWin32
    RUNTIME DESTINATION external/TspWin32/bin
    LIBRARY DESTINATION external/TspWin32/lib
    ARCHIVE DESTINATION external/TspWin32/lib
    )

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	## debug file ##
	IF (EXISTS ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/TspWin32.pdb)
		INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/TspWin32.pdb DESTINATION bin)
		INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/TspWin32.pdb DESTINATION external/TspWin32/bin)
	ENDIF (EXISTS ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}/TspWin32.pdb)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")