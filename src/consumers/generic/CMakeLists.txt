
SET(GENERIC_SRC 
  generic_consumer 
  generic_consumer_main)

TSP_XSCRIPT( 
  tsp_request_open tsp_request_close
  tsp_request_async_sample_read 
  tsp_request_async_sample_write
  tsp_request_information 
  tsp_request_filtered_information
  tsp_request_extended_information)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/consumers/generic)

ADD_EXECUTABLE(tsp_request_generic ${GENERIC_SRC})
	
TARGET_LINK_LIBRARIES(tsp_request_generic 
  tsp_consumer tsp_common tsp_services
  pthread)
