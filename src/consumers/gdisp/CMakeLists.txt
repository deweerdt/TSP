TSP_XSCRIPT( 
           stub_gdisp_config.xml
	   )

SET(GDISP_SRC interface
support gdispmain
config plotwindow
animation)

ADD_EXECUTABLE(tsp_gdisp ${GDISP_SRC})
TSP_PROGRAM_INSTALL(tsp_gdisp)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/consumers/gdisp/)
ADD_DEFINITIONS(${GTK_CFLAGS} -Wall)
TARGET_LINK_LIBRARIES(tsp_gdisp ${XML2_LIBRARY} ${GTK_LIBRARY}
${PTHREAD_LIBRARY_NAME} tsp_consumer tsp_common tsp_services
tsp_page_config)
