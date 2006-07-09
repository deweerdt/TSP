SET(_SRC
tspcfg_file.c)
ADD_DEFINITIONS(${XML2_CFLAGS})

ADD_LIBRARY(tspcfg ${_SRC})
ADD_EXECUTABLE(tsp_cfg tspcfg_file_main.c)
TARGET_LINK_LIBRARIES(tsp_cfg tspcfg ${XML2_LIBRARY} tsp_common)
