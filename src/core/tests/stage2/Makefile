include $(DEVBASE)/make/Makeparams.$(T)





LIB_CLIENT =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_rpc_client.a $(DIRLIB)/libtsp_services.a 
LIB_SERVEUR = $(DIRLIB)/libtsp_rpc_server.a $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_rpc_server.a  

all::
	$(MAKE)  common debug
common: 
	$(MAKE)  T=consumer$(D)	  build_client 
	$(MAKE)  T=provider$(D)   build_server
debug:
	$(MAKE)  D=.debug common


#client building 
build_client : $(DIRBIN)/tsp_client2

$(DIRBIN)/tsp_client2 :  $(LIB_CLIENT) client2.c
	$(COMPILE.c) client2.c -o $(DIRBIN)/client2.o
	$(LINK.c)   $(DIRBIN)/client2.o $(LIB_CLIENT) $(LDLIBS) -o $@

#serveur building 
build_server : $(DIRBIN)/tsp_server2

$(DIRBIN)/tsp_server2 :  $(LIB_SERVEUR) server2.c glue_sserver2.c
	$(COMPILE.c) server2.c  -o $(DIRBIN)/server2.o
	$(COMPILE.c) glue_sserver2.c  -o $(DIRBIN)/glue_sserver2.o
	$(LINK.c)   $(DIRBIN)/server2.o $(DIRBIN)/glue_sserver2.o $(LIB_SERVEUR) $(LDLIBS) -o $@


include $(DEVBASE)/make/Makerules
