include $(DEVBASE)/make/Makeparams

ifeq (${HOST_TARGET},java)
#	SUBDIRS = 
else
ifeq (${BUILD_BB},yes)
	SUBDIRS = libcalc libres libpages libbb
else 
	SUBDIRS = libcalc libres libpages
endif
endif

include $(DEVBASE)/make/Makesubdirs
