include $(DEVBASE)/make/Makeparams

SUBDIRS = libcalc libres libhisto libutil
ifeq (${BUILD_XML2_CONFIG_USER},yes)
	SUBDIRS += libpages libtspcfg
endif
ifeq (${BUILD_BB},yes)
	SUBDIRS += libbb
endif

include $(DEVBASE)/make/Makesubdirs
