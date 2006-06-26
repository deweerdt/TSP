SET(VISU3D_SRC Loader3ds glScene)
IF(USABLE_GLUT)
	SET(VISU3D_SRC ${VISU3D_SRC} testTSP)
ENDIF(USABLE_GLUT)

ADD_EXECUTABLE(Visu3D ${VISU3D_SRC})
	
TARGET_LINK_LIBRARIES(Visu3D m GLU GL glut X11 Xmu tsp_consumer tsp_common tsp_services)

ADD_DEPENDENCIES(Visu3D libtsp_common libtsp_consumer libtsp_services)
