#Just simple makefile for building in all subdirs
include $(DEVBASE)/make/Makeparams

SUBDIRS = \
	stdout_callback \
	stdout \
	test_group \
	res_writer \
	tutorial \
	ascii_writer

ifeq (${BUILD_GTK_BASED_CONSUMER},yes)
	SUBDIRS += gdisp gdisp+ 
endif

ifeq (${BUILD_JAVA},yes)
	SUBDIRS += jstdout
endif

include $(DEVBASE)/make/Makesubdirs


