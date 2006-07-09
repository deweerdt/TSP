SET(GDISP_SRC interface
support gdispmain
config plotwindow
animation)

ADD_EXECUTABLE(gdisp ${GDISP_SRC})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/consumers/gdisp/)
ADD_DEFINITIONS(${GTK_CFLAGS})
TARGET_LINK_LIBRARIES(gdisp ${XML2_LIBRARY} ${GTK_LIBRARY} tsp_consumer tsp_common tsp_services
pthread tsp_page_config)

