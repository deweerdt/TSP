
# Generate tsp_prjcfg header
TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
  include/tsp_prjcfg.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h)

TSP_XINCLUDE(${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h)

SET (INCLUDE_CORE_FILE 
	include/tsp_abs_types.h
	include/tsp_const_def.h
	include/tsp_glu.h
	include/tsp_simple_trace.h
	include/tsp_sys_headers.h
	rpc/tsp_rpc.h
	rpc/tsp_datastruct.h)
TSP_XINCLUDE(${INCLUDE_CORE_FILE})

#
# Services library
#
SET(SERVICES_LIB_SRCS
  ${SERVICES_LIB_SRCS}
  misc_utils/tsp_time
  misc_utils/tsp_mtgprof)

TSP_XINCLUDE(${SERVICES_LIB_SRCS} misc_utils/tsp_ringbuf misc_utils/tsp_services)
ADD_LIBRARY(tsp_services ${SERVICES_LIB_SRCS})
IF (WIN32) 
  TARGET_LINK_LIBRARIES(tsp_services ${PTHREAD_LIBRARY_NAME} Ws2_32 oncrpc)
ENDIF (WIN32)
TSP_LIBRARY_INSTALL(tsp_services)

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

TSP_XINCLUDE(${COMMON_LIB_SRCS} common/tsp_common_filter)

ADD_LIBRARY(tsp_common ${COMMON_LIB_SRCS})
IF(ACPLT_ONCRPC_FOUND)
	TARGET_LINK_LIBRARIES(tsp_common Ws2_32 oncrpc)
ENDIF(ACPLT_ONCRPC_FOUND)
TSP_LIBRARY_INSTALL(tsp_common) 
ADD_DEPENDENCIES(tsp_common rpcgen_exec_clnt)

# Setup RPC machinery

# FIXME Should write FindRPC CMake module
# Exec rpcgen
SET(RPCGEN "rpcgen")

SET(RPC_INPUT_FILE_PREFIX "rpc/tsp_rpc")
SET(RPC_GENERATED_DIR_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/rpc")
FILE(MAKE_DIRECTORY ${RPC_GENERATED_DIR_PREFIX})
SET(RPC_GENERATED_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/rpc/tsp_rpc")
#SET(RPC_GENERATED_FILE_PREFIX "${RPC_INPUT_FILE_PREFIX}")

SET(RPCGEN_OUTPUT_CNLT
  ${RPC_GENERATED_FILE_PREFIX}_clnt.c
  ${RPC_GENERATED_FILE_PREFIX}_xdr.c
  ${RPC_GENERATED_FILE_PREFIX}.h)

SET(RPCGEN_OUTPUT_SRV
  ${RPC_GENERATED_FILE_PREFIX}_svc.c
  ${RPC_GENERATED_FILE_PREFIX}_xdr.c
  ${RPC_GENERATED_FILE_PREFIX}.h)

ADD_CUSTOM_TARGET(rpcgen_exec_svc
  DEPENDS ${RPCGEN_OUTPUT_SRV}
  )
ADD_CUSTOM_TARGET(rpcgen_exec_clnt
  DEPENDS ${RPCGEN_OUTPUT_CNLT}
  )		  

IF (WIN32)
	
  # Currently NO RPCGEN on Windows 
  # Just using Linux generated files.
  CONFIGURE_FILE(${RPC_INPUT_FILE_PREFIX}_svc.c
	 	 ${RPC_GENERATED_FILE_PREFIX}_svc.c
		 COPYONLY)

  CONFIGURE_FILE(${RPC_INPUT_FILE_PREFIX}_clnt.c
	 	 ${RPC_GENERATED_FILE_PREFIX}_clnt.c
		 COPYONLY)

  CONFIGURE_FILE(${RPC_INPUT_FILE_PREFIX}_xdr.c
	 	 ${RPC_GENERATED_FILE_PREFIX}_xdr.c
		 COPYONLY)

  CONFIGURE_FILE(${RPC_INPUT_FILE_PREFIX}.h
	 	 ${RPC_GENERATED_FILE_PREFIX}.h
		 COPYONLY)

ELSE (WIN32)

#  Note how CMake forced us to escape '#' sequence using '\\#'
		    
  ADD_CUSTOM_COMMAND(
    OUTPUT ${RPC_GENERATED_FILE_PREFIX}_svc.c 
    COMMAND echo "\\#ifdef VXWORKS" >  ${RPC_GENERATED_FILE_PREFIX}_svc.c
    COMMAND echo "\\#define static extern" >> ${RPC_GENERATED_FILE_PREFIX}_svc.c
    COMMAND echo "\\#endif /* !VXWORKS */" >> ${RPC_GENERATED_FILE_PREFIX}_svc.c
    COMMAND ${RPCGEN} -N -C -m ${RPC_INPUT_FILE_PREFIX}.x >> ${RPC_GENERATED_FILE_PREFIX}_svc.c
    DEPENDS 
      ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h
      ${RPC_INPUT_FILE_PREFIX}.x
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	
  ADD_CUSTOM_COMMAND(
    OUTPUT ${RPC_GENERATED_FILE_PREFIX}_clnt.c 
    COMMAND ${RPCGEN} -N -C -l ${RPC_INPUT_FILE_PREFIX}.x > ${RPC_GENERATED_FILE_PREFIX}_clnt.c
    DEPENDS 
      ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h
      ${RPC_INPUT_FILE_PREFIX}.x
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
  ADD_CUSTOM_COMMAND(
    OUTPUT ${RPC_GENERATED_FILE_PREFIX}_xdr.c
    COMMAND ${RPCGEN} -N -C -c ${RPC_INPUT_FILE_PREFIX}.x > ${RPC_GENERATED_FILE_PREFIX}_xdr.c
    DEPENDS 
      ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h
      ${RPC_INPUT_FILE_PREFIX}.x
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
  ADD_CUSTOM_COMMAND(
    OUTPUT ${RPC_GENERATED_FILE_PREFIX}.h
    COMMAND ${RPCGEN} -N -C -h ${RPC_INPUT_FILE_PREFIX}.x > ${RPC_GENERATED_FILE_PREFIX}.h
    DEPENDS 
      ${CMAKE_CURRENT_BINARY_DIR}/include/tsp_prjcfg.h
      ${RPC_INPUT_FILE_PREFIX}.x
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

ENDIF (WIN32)

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
	
	TSP_XINCLUDE(${CONSUMER_LIB_SRCS})
	
	#RPC
	SET(CONSUMER_LIB_SRCS
	${CONSUMER_LIB_SRCS}
	${RPCGEN_OUTPUT_CNLT}
	rpc/tsp_client
	rpc/tsp_rpc_confprogid)	
	
	ADD_LIBRARY(tsp_consumer ${CONSUMER_LIB_SRCS})
	TSP_LIBRARY_INSTALL(tsp_consumer) 
	ADD_DEPENDENCIES(tsp_consumer tsp_common rpcgen_exec_clnt)
	IF (WIN32)
		TARGET_LINK_LIBRARIES(tsp_consumer tsp_services ${PTHREAD_LIBRARY_NAME} Ws2_32 oncrpc)
	ENDIF (WIN32)
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
	ctrl_init/tsp_provider_init)
	
	TSP_XINCLUDE(${PROVIDER_LIB_SRCS})
	
	SET(PROVIDER_LIB_SRCS
	${PROVIDER_LIB_SRCS}
	ctrl/tsp_request_handler
	ctrl/tsp_filter_symbol)	
	
	#RPC
	SET(PROVIDER_LIB_SRCS
	${PROVIDER_LIB_SRCS}
	${RPCGEN_OUTPUT_SRV}
	rpc/tsp_server
	rpc/tsp_rpc_confprogid)
    
    # PROVIDER LIB    
	ADD_LIBRARY(tsp_provider ${PROVIDER_LIB_SRCS})
	TSP_LIBRARY_INSTALL(tsp_provider) 
	ADD_DEPENDENCIES(tsp_provider tsp_common)
	IF (WIN32)		
		TARGET_LINK_LIBRARIES(tsp_provider tsp_services ${PTHREAD_LIBRARY_NAME} Ws2_32 oncrpc)
	ENDIF (WIN32)
	
	ADD_EXECUTABLE(tsp_rpc_cleanup rpc/tsp_rpc_cleanup.c rpc/tsp_rpc_confprogid)
	TSP_PROGRAM_INSTALL(tsp_rpc_cleanup)
	ADD_DEPENDENCIES(tsp_rpc_cleanup tsp_common tsp_provider tsp_services)
	IF (WIN32)		
		TARGET_LINK_LIBRARIES(tsp_rpc_cleanup tsp_services Ws2_32 oncrpc)		
	ENDIF (WIN32)	
ENDIF(BUILD_PROVIDER)