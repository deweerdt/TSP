SET(VISU3D_SRC Loader3ds glScene)
IF(GLUT_FOUND)
	SET(VISU3D_SRC ${VISU3D_SRC} testTSP)
ENDIF(GLUT_FOUND)

ADD_EXECUTABLE(Visu3D ${VISU3D_SRC})
TSP_PROGRAM_INSTALL(Visu3D)	

TARGET_LINK_LIBRARIES(Visu3D m GL GLU glut tsp_consumer tsp_common tsp_services $(PTHREAD_LIBRARY_NAME))

ADD_DEPENDENCIES(Visu3D libtsp_common libtsp_consumer libtsp_services)
