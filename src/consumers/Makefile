#Just simple makefile for building in all subdirs
include $(DEVBASE)/make/Makeparams

ifeq (${EXCLUDE_CONSUMER},yes)
	# nothing to do
else
	SUBDIRS = \
		stdout \
		test_group \
		res_writer \
		tutorial \
		gdisp
	ifeq (${EXCLUDE_JAVA},no)
		SUBDIRS += jstdout
	endif
	include $(DEVBASE)/make/Makesubdirs
endif

