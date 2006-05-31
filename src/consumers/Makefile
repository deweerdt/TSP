#Just simple makefile for building in all subdirs
include $(DEVBASE)/make/Makeparams

SUBDIRS = \
	stdout_callback \
	stdout \
	test_group \
	res_writer \
	tutorial \
	generic

ifeq (${BUILD_TSPFS},yes)
	SUBDIRS += tspfs
endif


ifeq (${BUILD_XML2_CONFIG_USER},yes)
	SUBDIRS += ascii_writer
ifeq (${BUILD_GTK_BASED_CONSUMER},yes)
	SUBDIRS += gdisp gdisp+
endif
endif

ifeq (${USABLE_OPENGL},yes) 
ifeq (${USABLE_OPENGLUT},yes) 
	SUBDIRS += Visu3D
endif
endif

include $(DEVBASE)/make/Makesubdirs


