include $(DEVBASE)/make/Makeparams
#----------------------------------------------------------
# core is only compiled for native host target and not java
#----------------------------------------------------------
	
SUBDIRS = include misc_utils

ifeq (${BUILD_XMLRPC}, yes)
	SUBDIRS += xmlrpc
else
	SUBDIRS += rpc
endif

ifeq (${BUILD_PROVIDER},yes)
	SUBDIRS += ctrl ctrl_init
endif
ifeq (${BUILD_CONSUMER},yes)
	SUBDIRS += driver
endif

include $(DEVBASE)/make/Makesubdirs

