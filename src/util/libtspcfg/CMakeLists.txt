
INSTALL(
  FILES 
  tspcfg_example.xml
  DESTINATION ${TSP_SHARE_BASEDIR}/example)

TSP_XINCLUDE(
  tspcfg_file.h
  )
SET(_SRC
tspcfg_file.c)
ADD_DEFINITIONS(${XML2_CFLAGS})

ADD_LIBRARY(tspcfg ${_SRC})
TSP_LIBRARY_INSTALL(tspcfg)
ADD_EXECUTABLE(tsp_cfg tspcfg_file_main.c)
TSP_PROGRAM_INSTALL(tsp_cfg)
TARGET_LINK_LIBRARIES(tsp_cfg tspcfg ${XML2_LIBRARY} tsp_common)
