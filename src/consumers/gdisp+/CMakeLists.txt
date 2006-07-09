SET(TARGA_SRC
gdisp_main gdisp_kernel
gdisp_preferences gdisp_configuration
gdisp_mainBoard gdisp_pilotBoard
gdisp_animatedLogo gdisp_dataBook
gdisp_hosts gdisp_providers
gdisp_consumers gdisp_symbols
gdisp_sampledSymbols gdisp_graphics
gdisp_pages gdisp_sampling
gdisp_colormap gdisp_defaultPlot
gdisp_pointArray gdisp_splashScreen
gdisp_pixmaps)

SET(TARGA_SRC ${TARGA_SRC}
gdisp_utils gdisp_toString
gdisp_xml gdisp_format
gdisp_dataBox gdisp_popupMenu
gdisp_plot2D gdisp_plotText)

IF(USABLE_OPENGL)
	SET(TARGA_SRC ${TARGA_SRC}
	gdisp_plotOrbital gdisp_openglUtils)
ENDIF(USABLE_OPENGL)

ADD_EXECUTABLE(targa ${TARGA_SRC})

ADD_DEFINITIONS(${GTK_CFLAGS})
ADD_DEFINITIONS(${XML2_CFLAGS})
TARGET_LINK_LIBRARIES(targa ${XML2_LIBRARY} ${GTK_LIBRARY} tsp_consumer tsp_common tsp_services
tsp_util gtk_1.2_utils)
ADD_DEPENDENCIES(targa tsp_consumer tsp_common tsp_services tsp_util
gtk_1.2_utils)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/gtk-1.2/)

IF(SHAREDLIB_ENABLED)
	ADD_LIBRARY(tsp_gdispUtils gdisp_utils gdisp_toString gdisp_xml gdisp_format)
	ADD_LIBRARY(tsp_gdispPlot2D gdisp_plot2D)
	ADD_LIBRARY(tsp_gdispPlotText gdisp_plotText)
	IF(USABLE_OPENGL)
		ADD_LIBRARY(tsp_gdispPlotOrbital gdisp_plotOrbital gdisp_openglUtils)
		TARGET_LINK_LIBRARIES(tsp_gdispPlotOrbital GL GLU)
	ENDIF(USABLE_OPENGL)
ENDIF(SHAREDLIB_ENABLED)
