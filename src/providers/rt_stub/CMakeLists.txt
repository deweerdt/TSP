
ADD_EXECUTABLE(tsp_rt_stub rtc_util.c stub_loopOnRtc.c main_stub.c)
TSP_PROGRAM_INSTALL(tsp_rt_stub)
ADD_DEPENDENCIES(tsp_rt_stub tsp_calc tsp_common tsp_services tsp_provider)

ADD_EXECUTABLE(bb_rt_stub rtc_util.c bb_vars.c main_stub.c)
TSP_PROGRAM_INSTALL(bb_rt_stub)
ADD_DEPENDENCIES(bb_rt_stub tsp_calc tsp_common tsp_services tsp_provider)


SET(LIBS tsp_calc tsp_histo bb tsp_provider tsp_common tsp_services)
IF (BUILD_XMLRPC)
    ADD_DEFINITIONS(${LIBWWW_CFLAGS})
    SET(LIBS ${LIBS} ${XMLRPCC_ABYSS_SERVER_LIBRARIES} ${LIBWWW_LIBRARIES})
ENDIF (BUILD_XMLRPC)

# AP : Migration sous Windows
IF (NOT WIN32)
	SET(LIBS ${LIBS} m)
ENDIF (NOT WIN32)

SET(LIBS ${LIBS} ${PTHREAD_LIBRARY_NAME} ${TSP_WIN32_LIB})
	
TARGET_LINK_LIBRARIES(tsp_rt_stub ${LIBS})
TARGET_LINK_LIBRARIES(bb_rt_stub ${LIBS})

