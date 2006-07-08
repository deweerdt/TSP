SET(LIBUTIL_SRC
tsp_hash.c)

ADD_LIBRARY(tsp_util ${LIBUTIL_SRC})
ADD_EXECUTABLE(tsp_hash_dump tsp_hash.c tsp_hash_main.c)

ADD_EXECUTABLE(tsp_check_host_and_ip tsp_check_host_and_ip.c)
