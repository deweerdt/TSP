# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams

RPC = TSP_rpc

RPC_CLIENT_SRC = TspRequestSender.java
#RPC_SERVER_SRC = TspServer.java
RPC_SERVER_SRC = 

JPACKAGE=tsp.core.rpc
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

all::
	$(MAKE) common debug
common:
	$(MAKE) rpcgen
	$(MAKE) rpc.client.bin
	$(MAKE) rpc.server.bin

debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

clean::
	$(MAKE) destroy clean_debug

clean_debug:
	echo "No debug for yet"
#	$(MAKE) D=.debug destroy

destroy::
	$(MAKE) rpc.clean

include $(DEVBASE)/make/Makerules.java


