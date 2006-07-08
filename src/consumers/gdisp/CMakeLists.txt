SET(GDISP_SRC interface
support gdispmain
config plotwindow
animation)

ADD_EXECUTABLE(gdisp ${GDISP_SRC})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/consumers/gdisp/)
ADD_DEFINITIONS(${GTK_FLAGS})
TARGET_LINK_LIBRARIES(gdisp ${XML2_LIBS} ${GTK_LIBS} tsp_consumer tsp_common tsp_services
pthread tsp_page_config)

