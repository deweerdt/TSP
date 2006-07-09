SET(VISU3D_SRC Loader3ds glScene)
IF(GLUT_FOUND)
	SET(VISU3D_SRC ${VISU3D_SRC} testTSP)
ENDIF(GLUT_FOUND)

ADD_EXECUTABLE(Visu3D ${VISU3D_SRC})
	
TARGET_LINK_LIBRARIES(Visu3D m GL GLU glut X11 Xmu tsp_consumer tsp_common tsp_services)

ADD_DEPENDENCIES(Visu3D libtsp_common libtsp_consumer libtsp_services)
