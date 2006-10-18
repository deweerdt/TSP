INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libcalc)
ADD_EXECUTABLE(tsp_stub_server glue_stub.c server_main.c)

SET(LIBS tsp_calc tsp_provider tsp_common tsp_services)
IF (BUILD_XMLRPC)
    ADD_DEFINITIONS(${LIBWWW_CFLAGS})
    SET(LIBS ${LIBS} ${XMLRPCC_ABYSS_SERVER_LIBRARIES} ${LIBWWW_LIBRARIES})
ENDIF (BUILD_XMLRPC)

# AP : Migration sous Windows
IF (NOT WIN32)
	SET(LIBS ${LIBS} m)
ENDIF (NOT WIN32)

SET(LIBS ${LIBS} ${PTHREAD_LIBRARY_NAME} ${TSP_WIN32_LIB})
	
TARGET_LINK_LIBRARIES(tsp_stub_server ${LIBS})

ADD_DEPENDENCIES(tsp_stub_server tsp_calc tsp_common tsp_services tsp_provider)
TSP_PROGRAM_INSTALL(tsp_stub_server)
