SET(_SRC
tspcfg_file.c)
ADD_DEFINITIONS(${XML2_FLAGS})

ADD_LIBRARY(tspcfg ${_SRC})
ADD_EXECUTABLE(tsp_cfg tspcfg_file_main.c)
TARGET_LINK_LIBRARIES(tsp_cfg tspcfg ${XML2_LIBS} tsp_common)
