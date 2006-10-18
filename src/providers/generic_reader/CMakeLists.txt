
ADD_EXECUTABLE(tsp_generic_reader 
  macsim_fmt.c
  glu_genreader.c 
  generic_reader.c 
  generic_reader_main.c)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(LIBS tsp_provider tsp_common tsp_services)
ADD_DEPENDENCIES(tsp_generic_reader ${LIBS} ${TSP_WIN32_LIB})

IF (BUILD_XMLRPC)
    ADD_DEFINITIONS(${LIBWWW_CFLAGS})
    SET(LIBS ${LIBS} ${XMLRPCC_ABYSS_SERVER_LIBRARIES} ${LIBWWW_LIBRARIES})
ENDIF (BUILD_XMLRPC)

SET(LIBS ${LIBS} ${PTHREAD_LIBRARY_NAME} ${TSP_WIN32_LIB})
	
TARGET_LINK_LIBRARIES(tsp_generic_reader ${LIBS})

TSP_PROGRAM_INSTALL(tsp_generic_reader)
