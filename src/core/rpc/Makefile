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
clean::
	$(MAKE) T=provider$(D) RPC_LIB_SUFFIX=_client rpc.clean
	$(MAKE) T=consumer$(D) RPC_LIB_SUFFIX=_server rpc.clean

debug::
	$(MAKE) D=.debug	common

print:
	@for file in Makefile tsp_rpc.x $(RPC_CLIENT_SRC) $(RPC_SERVER_SRC); \
	 do \
		echo "printing $$file..."; \
		il $$file; \
	 done


include $(DEVBASE)/make/Makerules












