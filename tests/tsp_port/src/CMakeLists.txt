
SET(TSP_PORT_SRC test)
SET(TSP_PORT_SVC_SRC test_server)
SET(TSP_PORT_API_SRC test_api)

# Setup RPC machinery
ONCRPC_SETUP("test_port_rpc"
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}/test_port_rpc"
  "")

SET(TSP_PORT_RPC_SVC_SRC test_rpc_server ${test_port_rpc_RPCGEN_OUTPUT_SVC})
SET(TSP_PORT_RPC_CLNT_SRC test_rpc_client ${test_port_rpc_RPCGEN_OUTPUT_CLNT})

IF (WIN32)
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/TspWin32)
ENDIF (WIN32)

# Libraries
TSP_XINCLUDE(${TSP_PORT_API_SRC})
ADD_LIBRARY(tsp_port_api ${TSP_PORT_API_SRC})
IF (PTHREADWIN32_FOUND)
  TARGET_LINK_LIBRARIES(tsp_port_api ${PTHREAD_LIBRARY_NAME} Ws2_32)
ENDIF (PTHREADWIN32_FOUND)

TSP_XINCLUDE(test_rpc_server)
ADD_LIBRARY(tsp_port_rpc_svc ${TSP_PORT_RPC_SVC_SRC})
IF (ACPLT_ONCRPC_FOUND)
  TARGET_LINK_LIBRARIES(tsp_port_rpc_svc tsp_port_api oncrpc TspWin32)
ENDIF (ACPLT_ONCRPC_FOUND)

TSP_XINCLUDE(test_rpc_client)
ADD_LIBRARY(tsp_port_rpc_clnt ${TSP_PORT_RPC_CLNT_SRC})
IF(ACPLT_ONCRPC_FOUND)
  TARGET_LINK_LIBRARIES(tsp_port_rpc_clnt tsp_port_api oncrpc ${TSP_WIN32_LIB})
ENDIF(ACPLT_ONCRPC_FOUND)

# Executable
ADD_EXECUTABLE(tsp_port_test ${TSP_PORT_SRC})
TARGET_LINK_LIBRARIES(tsp_port_test 
  tsp_port_api 
  tsp_port_rpc_clnt 
  ${PTHREAD_LIBRARY_NAME} 
  ${ONCRPC_LIBRARY} 
  ${SOCKET_LIBRARY})
ADD_DEPENDENCIES(tsp_port_test tsp_port_rpc_clnt)

ADD_EXECUTABLE(tsp_port_test_svc ${TSP_PORT_SVC_SRC})
TARGET_LINK_LIBRARIES(tsp_port_test_svc 
  tsp_port_api
  tsp_port_rpc_svc 
  ${PTHREAD_LIBRARY_NAME}
  ${TSP_WIN32_LIB}
  ${ONCRPC_LIBRARY}
  ${SOCKET_LIBRARY})
ADD_DEPENDENCIES(tsp_port_test_svc tsp_port_rpc_svc)

TSP_PROGRAM_INSTALL(tsp_port_test tsp_port_test_svc)
TSP_LIBRARY_INSTALL(tsp_port_rpc_svc tsp_port_rpc_clnt tsp_port_api)
