SET(EXE_NAME tsp_testgrp_client)
SET(SRCS client_group)

ADD_EXECUTABLE(${EXE_NAME} ${SRCS})
TSP_PROGRAM_INSTALL(${EXE_NAME})

TARGET_LINK_LIBRARIES(${EXE_NAME} tsp_consumer tsp_common tsp_services
tsp_calc pthread)

ADD_DEPENDENCIES(${EXE_NAME} tsp_calc tsp_common tsp_consumer tsp_services)



