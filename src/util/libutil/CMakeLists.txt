SET(LIBUTIL_SRC
tsp_hash.c)

ADD_LIBRARY(tsp_util ${LIBUTIL_SRC})
ADD_EXECUTABLE(tsp_hash_dump tsp_hash.c)
#SET_TARGET_PROPERTIES(tsp_hash_dump PROPERTIES CMAKE_C_FLAGS -DMAIN)
SET_TARGET_PROPERTIES(tsp_hash_dump PROPERTIES COMPILE_FLAGS "-DMAIN")
ADD_EXECUTABLE(tsp_check_host_and_ip tsp_check_host_and_ip.c)
