include $(DEVBASE)/make/Makeparams.$(T)

XINC = tsp_rpc.h tsp_client.h tsp_datastruct.h tsp_server.h

RPC = tsp_rpc


RPCGENFLAGS=  -N -C

RPC_CLIENT_SRC = tsp_client.c tsp_rpc_confprogid.c
RPC_SERVER_SRC = tsp_server.c tsp_rpc_confprogid.c


all::
	$(MAKE) common debug
common:
	$(MAKE) T=provider$(D) rpcgen
	$(MAKE) T=provider$(D) RPC_LIB_SUFFIX=_server rpc.server.lib
	$(MAKE) T=consumer$(D) RPC_LIB_SUFFIX=_client rpc.client.lib 

debug::
	$(MAKE) D=.debug	common



clean::
	$(MAKE) destroy clean_debug

clean_debug:
	$(MAKE) D=.debug destroy

destroy::
	$(MAKE) T=provider$(D) RPC_LIB_SUFFIX=_client rpc.clean
	$(MAKE) T=consumer$(D) RPC_LIB_SUFFIX=_server rpc.clean



include $(DEVBASE)/make/Makerules












