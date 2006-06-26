SET(RESW_SRC client_res)

ADD_EXECUTABLE(tsp_res_writer ${RESW_SRC})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libres)

TARGET_LINK_LIBRARIES(tsp_res_writer tsp_consumer tsp_common tsp_services
tsp_res pthread)

ADD_DEPENDENCIES(tsp_res_writer libtsp_res 
libtsp_common libtsp_consumer libtsp_services)
