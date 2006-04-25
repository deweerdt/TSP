include $(DEVBASE)/make/Makeparams

SUBDIRS = stub res_reader generic_reader
ifeq (${BUILD_BB},yes)
	SUBDIRS += bb_provider
endif

ifeq (${TSP_TARGET},vxworks)
	SUBDIRS += vxstub
endif

include $(DEVBASE)/make/Makesubdirs


