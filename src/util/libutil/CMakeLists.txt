SET(LIBUTIL_SRC
tsp_hash.c)

ADD_LIBRARY(tsp_util ${LIBUTIL_SRC})
TSP_LIBRARY_INSTALL(tsp_util)
ADD_EXECUTABLE(tsp_hash_dump tsp_hash.c tsp_hash_main.c)
TSP_PROGRAM_INSTALL(tsp_hash_dump)
ADD_EXECUTABLE(tsp_check_host_and_ip tsp_check_host_and_ip.c)
TSP_PROGRAM_INSTALL(tsp_check_host_and_ip)
IF (CMAKE_SYSTEM_NAME MATCHES "SunOS")
  TARGET_LINK_LIBRARIES(tsp_check_host_and_ip nsl)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "SunOS")