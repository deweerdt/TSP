include $(DEVBASE)/make/Makeparams

ifeq (${HOST_TARGET},java)
#	SUBDIRS = 
else
SUBDIRS = libcalc libres
ifeq (${BUILD_XML2_CONFIG_USER},yes)
	SUBDIRS += libpages
endif
ifeq (${BUILD_BB},yes)
	SUBDIRS += libbb
endif
endif

include $(DEVBASE)/make/Makesubdirs
