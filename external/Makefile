include $(DEVBASE)/make/Makeparams

#--------------------------------------------------
# compile rpcgen and posix stubs for VXWORKS target
#--------------------------------------------------
ifeq (${TSP_TARGET},vxworks)
SUBDIRS = RpcGen VxWorks
endif

#--------------------------------------------------
# compile rpcgen for DEC target
#--------------------------------------------------
ifeq (${TSP_TARGET},dec)
SUBDIRS = RpcGen
endif

include $(DEVBASE)/make/Makesubdirs

