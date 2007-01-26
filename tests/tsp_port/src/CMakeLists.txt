
SET(TSP_PORT_SRC test)
SET(TSP_PORT_SVC_SRC test_server)
SET(TSP_PORT_API_SRC test_api)
SET(TSP_PORT_RPC_SVC_SRC test_rpc_server test_port_rpc_xdr test_port_rpc_svc)
SET(TSP_PORT_RPC_CLNT_SRC test_rpc_client test_port_rpc_xdr test_port_rpc_clnt)

# Exec rpcgen 
SET(RPCGEN ${RPCGEN_PROGRAM})

MESSAGE(STATUS "rpcgen : - found rpcgen is ${RPCGEN_PROGRAM}")

SET(RPC "test_port_rpc")
SET(TEST_PORT_SRC ${TEST_PORT_SRC})

###############################
# RPCGEN ne fonctionne pas actuellement dans Cmake, 
# il faut le faire dans une "Visual Studio .Net 2003 command prompt"
###############################
#ADD_CUSTOM_COMMAND(
#	OUTPUT	${RPC}_xdr.c
#	COMMAND ${RPCGEN} -c ${RPC}.x -o ${RPC}_xdr.c
#	DEPENDS ${RPC}.x
#	WORKING_DIRECTORY ${CMAKE_CURRENT_DIR})
	
#ADD_CUSTOM_COMMAND(
#	OUTPUT	${RPC}.h
#	COMMAND ${RPCGEN} -h ${CMAKE_SOURCE_DIR}/tests/tsp_port/src/${RPC}.x -o ${RPC}.h
#	DEPENDS ${CMAKE_SOURCE_DIR}/tests/tsp_port/src/${RPC}.x)

#ADD_CUSTOM_COMMAND(
#	OUTPUT 	${RPC}_clnt.c
#	COMMAND ${RPCGEN} -l ${CMAKE_SOURCE_DIR}/tests/tsp_port/src/${RPC}.x -o ${RPC}_clnt.c
#	DEPENDS ${CMAKE_SOURCE_DIR}/tests/tsp_port/src/${RPC}.x)

#ADD_CUSTOM_COMMAND(
#	OUTPUT 	${RPC}_scv.c
#	COMMAND ${RPCGEN} -m ${CMAKE_SOURCE_DIR}/tests/tsp_port/src/${RPC}.x -o ${RPC}_svc.c
#	DEPENDS ${CMAKE_SOURCE_DIR}/tests/tsp_port/src/${RPC}.x)

IF (WIN32)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/TspWin32)
ENDIF (WIN32)

# DLL
TSP_XINCLUDE(${TSP_PORT_API_SRC})
ADD_LIBRARY(tsp_port_api ${TSP_PORT_API_SRC})
IF(PTHREADWIN32_FOUND)
	TARGET_LINK_LIBRARIES(tsp_port_api ${PTHREAD_LIBRARY_NAME} Ws2_32)
ENDIF(PTHREADWIN32_FOUND)

TSP_XINCLUDE(test_rpc_server)
ADD_LIBRARY(tsp_port_rpc_svc ${TSP_PORT_RPC_SVC_SRC})
IF(ACPLT_ONCRPC_FOUND)
	TARGET_LINK_LIBRARIES(tsp_port_rpc_svc tsp_port_api oncrpc TspWin32)
ENDIF(ACPLT_ONCRPC_FOUND)

TSP_XINCLUDE(test_rpc_client)
ADD_LIBRARY(tsp_port_rpc_clnt ${TSP_PORT_RPC_CLNT_SRC})
IF(ACPLT_ONCRPC_FOUND)
	TARGET_LINK_LIBRARIES(tsp_port_rpc_clnt tsp_port_api oncrpc ${TSP_WIN32_LIB})
ENDIF(ACPLT_ONCRPC_FOUND)

# EXE
ADD_EXECUTABLE(tsp_port_test ${TSP_PORT_SRC})
TARGET_LINK_LIBRARIES(tsp_port_test tsp_port_api tsp_port_rpc_clnt ${PTHREAD_LIBRARY_NAME} ${RPC_LIBRARY_NAME} ${SOCKET_LIBRARY_NAME})
ADD_DEPENDENCIES(tsp_port_test tsp_port_rpc_clnt)

ADD_EXECUTABLE(tsp_port_test_svc ${TSP_PORT_SVC_SRC})
TARGET_LINK_LIBRARIES(tsp_port_test_svc tsp_port_api tsp_port_rpc_svc ${PTHREAD_LIBRARY_NAME}  ${TSP_WIN32_LIB} ${RPC_LIBRARY_NAME} ${SOCKET_LIBRARY_NAME})
ADD_DEPENDENCIES(tsp_port_test_svc tsp_port_rpc_svc)

TSP_PROGRAM_INSTALL(tsp_port_test tsp_port_test_svc)
TSP_LIBRARY_INSTALL(tsp_port_rpc_svc tsp_port_rpc_clnt tsp_port_api)
