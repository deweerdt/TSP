#Just simple makefile for building in all subdirs
include $(DEVBASE)/make/Makeparams

ifeq (${EXCLUDE_CONSUMER},yes)
	# nothing to do
else
	SUBDIRS = \
		stdout_callback \
		stdout \
		test_group \
		res_writer \
		tutorial \
		gdisp
	ifeq (${BUILD_JAVA},yes)
		SUBDIRS += jstdout jsynoptic
	endif
	include $(DEVBASE)/make/Makesubdirs
endif

