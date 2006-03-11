include $(DEVBASE)/make/Makeparams


SUBDIRS = core util 

ifeq (${BUILD_PROVIDER},yes)
	SUBDIRS += providers
endif

ifeq (${BUILD_CONSUMER},yes)
	SUBDIRS += consumers
endif

ifeq (${APIDOC_GEN},yes)
	SUBDIRS += doxy
endif


include $(DEVBASE)/make/Makesubdirs

