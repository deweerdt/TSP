SET(EXE_NAME tsp_stdout_client_callback)
SET(STDOUT_SRC client_callback_stdout)

ADD_EXECUTABLE(${EXE_NAME} ${STDOUT_SRC})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libcalc)

TARGET_LINK_LIBRARIES(${EXE_NAME} tsp_consumer tsp_common tsp_services
tsp_calc m pthread)

ADD_DEPENDENCIES(${EXE_NAME} tsp_calc tsp_common tsp_consumer tsp_services)


