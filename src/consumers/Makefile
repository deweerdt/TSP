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
ifeq (${XML2_CONFIG_USER},yes)
	SUBDIRS += gdisp gdisp+ 
endif
endif

include $(DEVBASE)/make/Makesubdirs


