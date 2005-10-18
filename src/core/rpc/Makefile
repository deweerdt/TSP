include $(DEVBASE)/make/Makeparams

XINC = tsp_rpc.h tsp_datastruct.h

RPC = tsp_rpc


RPCGENFLAGS=  -N -C

RPC_CLIENT_SRC = tsp_client.c tsp_rpc_confprogid.c
RPC_SERVER_SRC = tsp_server.c tsp_rpc_confprogid.c

RPC_CLEANUP = $(DIRBIN)/tsp_rpc_cleanup


# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: export_include
	$(MAKE) rpcgen
	@if [ "${BUILD_PROVIDER}" = "yes" ]; then \
		$(MAKE) RPC_LIB_SUFFIX=tsp_provider rpc.server.lib ;  \
		$(MAKE) "CFLAGS += -DTSP_RPC_CLEANUP" \
			"LDLIBS += -ltsp_provider" main_cleanup ; \
	fi ;\
	if [ "${BUILD_CONSUMER}" = "yes" ]; then \
		$(MAKE) RPC_LIB_SUFFIX=tsp_consumer rpc.client.lib;   \
	fi

destroy::
	$(MAKE) RPC_LIB_SUFFIX=tsp_consumer rpc.clean
	$(MAKE) RPC_LIB_SUFFIX=tsp_provider rpc.clean
	$(MAKE) remove_include


main_cleanup : $(RPC_CLEANUP)

$(RPC_CLEANUP) : tsp_server
	mv $< $@

include $(DEVBASE)/make/Makerules





