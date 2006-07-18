#
# Services library
#
SET(SERVICES_LIB_SRCS
  ${SERVICES_LIB_SRCS}
  misc_utils/tsp_time
  misc_utils/tsp_mtgprof)

ADD_LIBRARY(tsp_services ${SERVICES_LIB_SRCS})

#
# Common library
#
SET(COMMON_LIB_SRCS
  ${COMMON_LIB_SRCS}
  common/tsp_common
  common/tsp_common_request
  common/tsp_common_ssi
  common/tsp_common_ssei
  common/tsp_common_as
  common/tsp_common_status
  common/tsp_encoder
  common/tsp_decoder)

ADD_LIBRARY(tsp_common ${COMMON_LIB_SRCS})
ADD_DEPENDENCIES(tsp_common tsp_consumer)

SET(RPCGEN_OUTPUT_CNLT
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc_clnt
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc_xdr
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc.h)

SET(RPCGEN_OUTPUT_SRV
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc_svc.c
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc_xdr.c
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc.h)

# Generate tsp_prjcfg header
TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  include/tsp_prjcfg.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h)

# FIXME Should write FindRPC CMake module
# Exec rpcgen
SET(RPCGEN "rpcgen")
SET(RPC "rpc/tsp_rpc")
ADD_CUSTOM_COMMAND(
  OUTPUT 
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc_svc.c 
  COMMAND echo "#ifdef VXWORKS" > ${RPC}_svc.c
  COMMAND echo "#define static extern" >> ${RPC}_svc.c
  COMMAND echo "#endif /* !VXWORKS */" >> ${RPC}_svc.c
  COMMAND ${RPCGEN} -N -C -m ${RPC}.x >> ${RPC}_svc.c
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h)

ADD_CUSTOM_COMMAND(
  OUTPUT 
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc_clnt.c 
  COMMAND ${RPCGEN} -N -C -l ${RPC}.x > ${RPC}_clnt.c
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h)

ADD_CUSTOM_COMMAND(
  OUTPUT 
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc_xdr.c
  COMMAND ${RPCGEN} -N -C -c ${RPC}.x > ${RPC}_xdr.c
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h)

ADD_CUSTOM_COMMAND(
  OUTPUT 
  ${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc.h
  COMMAND ${RPCGEN} -N -C -h ${RPC}.x > ${RPC}.h
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h)

#
#Consumer library
#
IF(BUILD_CONSUMER)
	SET(CONSUMER_LIB_SRCS
	${CONSUMER_LIB_SRCS}
	driver/tsp_data_receiver
	driver/tsp_consumer
	driver/tsp_group
	driver/tsp_stream_receiver)
	#RPC
	SET(CONSUMER_LIB_SRCS
	${CONSUMER_LIB_SRCS}
	${RPCGEN_OUTPUT_CNLT}
	rpc/tsp_client
	rpc/tsp_rpc_confprogid)

	ADD_LIBRARY(tsp_consumer ${CONSUMER_LIB_SRCS})
	#ADD_DEPENDENCIES(tsp_consumer)
ENDIF(BUILD_CONSUMER)

#
#Provider library
#
IF(BUILD_PROVIDER)

    #XMLRPC
    IF (BUILD_XMLRPC)
        ADD_DEFINITIONS( ${XMLRPCC_CFLAGS} )        
        SET(PROVIDER_LIB_SRCS
            ${PROVIDER_LIB_SRCS}
            xmlrpc/tsp_xmlrpc_server
            xmlrpc/tsp_xmlrpc_util)	
        ADD_DEFINITIONS(-DBUILD_XMLRPC)        
    ENDIF (BUILD_XMLRPC)  
      
    # PROVIDER SRC
	SET(PROVIDER_LIB_SRCS
	${PROVIDER_LIB_SRCS}
	ctrl/tsp_default_glu
	ctrl/tsp_provider
	ctrl/tsp_session
	ctrl/tsp_datapool
	ctrl/tsp_group_algo
	ctrl/tsp_data_sender
	ctrl/tsp_stream_sender
	ctrl/tsp_request_handler
	ctrl/tsp_filter_symbol
	ctrl_init/tsp_provider_init)

	#RPC
	SET(PROVIDER_LIB_SRCS
	${PROVIDER_LIB_SRCS}
	${RPCGEN_OUTPUT_SRV}
	rpc/tsp_server
	rpc/tsp_rpc_confprogid)
    
    # PROVIDER LIB
	ADD_LIBRARY(tsp_provider ${PROVIDER_LIB_SRCS})

ENDIF(BUILD_PROVIDER)
