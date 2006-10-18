SET(EXE_NAME tsp_stdout_client)

SET(STDOUT_SRC client_stdout)

IF (WIN32)
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/TspWin32)
ENDIF (WIN32)

ADD_EXECUTABLE(${EXE_NAME} ${STDOUT_SRC})

IF (WIN32)
	TARGET_LINK_LIBRARIES(${EXE_NAME} tsp_consumer tsp_common tsp_services
	tsp_calc ${PTHREAD_LIBRARY_NAME} ${TSP_WIN32_LIB})
ELSE (WIN32)
	TARGET_LINK_LIBRARIES(${EXE_NAME} tsp_consumer tsp_common tsp_services
	tsp_calc m ${PTHREAD_LIBRARY_NAME})
ENDIF (WIN32)

ADD_DEPENDENCIES(${EXE_NAME} tsp_calc tsp_common tsp_consumer tsp_services)

TSP_PROGRAM_INSTALL(${EXE_NAME})
