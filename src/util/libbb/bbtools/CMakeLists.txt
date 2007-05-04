SET(SIMU_SRC bb_simu)
SET(TOOLS_SRC bb_tools_main)

TSP_XSCRIPT( 
  bb_findlabel 
  bb_find
  bb_dump
  bb_load
  bb_read 
  bb_write
  bb_check_id 
  bb_destroy
  bb_synchro_send 
  bb_synchro_recv 
  bb_memset 
  bb_check_version 
  bb_publish 
  bb_create
  bb_help
  )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/util/libbb)

ADD_EXECUTABLE(bb_simu ${SIMU_SRC})
TARGET_LINK_LIBRARIES(bb_simu bb m)
TSP_PROGRAM_INSTALL(bb_simu)

ADD_EXECUTABLE(bb_tools ${TOOLS_SRC})
TARGET_LINK_LIBRARIES(bb_tools bb)
TSP_PROGRAM_INSTALL(bb_tools)
