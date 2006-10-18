SET(RESW_SRC client_res)

ADD_EXECUTABLE(tsp_res_writer ${RESW_SRC})
TSP_PROGRAM_INSTALL(tsp_res_writer)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libres)

TARGET_LINK_LIBRARIES(tsp_res_writer tsp_consumer tsp_common tsp_services
tsp_res pthread)

ADD_DEPENDENCIES(tsp_res_writer tsp_res 
tsp_common tsp_consumer tsp_services)
