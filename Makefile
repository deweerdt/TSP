include $(DEVBASE)/make/Makeparams
#----------------------------------------------------------
# core is only compiled for native host target and not java
#----------------------------------------------------------

SUBDIRS = src

include $(DEVBASE)/make/Makesubdirs

