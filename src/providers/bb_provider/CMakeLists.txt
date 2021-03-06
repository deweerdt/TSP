SET(_EXE tsp_bb_provider)
SET(_LIB bb_tsp_provider)
SET(LIB_SRC bb_tsp_provider.c) 
SET(TSPTARGET "linux")

IF(TSPTARGET STREQUAL "vxworks")
	SET(EXE_SRC bb_vx_provider.c) 
ELSE(TSPTARGET STREQUAL "vxworks")
	SET(EXE_SRC bb_tsp_provider_main.c)
ENDIF(TSPTARGET STREQUAL "vxworks")

ADD_EXECUTABLE(${_EXE} ${EXE_SRC})
TSP_PROGRAM_INSTALL(${_EXE})

ADD_LIBRARY(${_LIB} ${LIB_SRC})
TSP_LIBRARY_INSTALL(${_LIB})

ADD_DEPENDENCIES(${_LIB} bb)
ADD_DEPENDENCIES(${_EXE} tsp_provider)
IF (BUILD_XMLRPC)
    ADD_DEFINITIONS(${LIBWWW_CFLAGS})
    TARGET_LINK_LIBRARIES(${_EXE}  ${XMLRPCC_ABYSS_SERVER_LIBRARIES} ${LIBWWW_LIBRARIES} )
ENDIF (BUILD_XMLRPC)    

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src/util/libbb
  ${CMAKE_SOURCE_DIR}/src/providers/bb_provider)
IF(TSPTARGET STREQUAL "vxworks")
	#TODO: Find a way to generate the .o
ELSE(TSPTARGET STREQUAL "vxworks")
	SET(EXE_SRC bb_tsp_provider_main.c)
	TARGET_LINK_LIBRARIES(${_EXE} bb_tsp_provider)
	TARGET_LINK_LIBRARIES(${_LIB} bb_tsp_provider tsp_provider tsp_common tsp_services bb tsp_calc)
ENDIF(TSPTARGET STREQUAL "vxworks")
