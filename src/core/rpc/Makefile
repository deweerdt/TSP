include $(DEVBASE)/make/Makeparams

XINC = tsp_rpc.h tsp_datastruct.h  # FIXME : shoudl not need to export tsp_datastruct.h
#XINC = tsp_rpc.h tsp_client.h tsp_datastruct.h tsp_server.h

RPC = tsp_rpc


RPCGENFLAGS=  -N -C

RPC_CLIENT_SRC = tsp_client.c tsp_rpc_confprogid.c
RPC_SERVER_SRC = tsp_server.c tsp_rpc_confprogid.c


# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::
	$(MAKE) rpcgen
	if [ "${BUILD_PROVIDER}" = "yes" ]; then \
		$(MAKE) RPC_LIB_SUFFIX=tsp_provider rpc.server.lib ;  \
	fi ;\
	if [ "${BUILD_CONSUMER}" = "yes" ]; then \
		$(MAKE) RPC_LIB_SUFFIX=tsp_consumer rpc.client.lib;   \
	fi
	$(MAKE) export_include

destroy::
	$(MAKE) RPC_LIB_SUFFIX=tsp_consumer rpc.clean
	$(MAKE) RPC_LIB_SUFFIX=tsp_provider rpc.clean
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules





