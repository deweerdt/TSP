include $(DEVBASE)/make/Makeparams.$(T)


LIB_CLIENT =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_rpc_client.a $(DIRLIB)/libtsp_services.a 
LIB_SERVER =  $(DIRLIB)/libtsp_provider_init.a $(DIRLIB)/libtsp_rpc_server.a  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a  



all::
	$(MAKE)  common debug
common: 
	$(MAKE)  T=consumer$(D)	  build_client 
	$(MAKE)  T=provider$(D)   build_server
debug:
	$(MAKE)  D=.debug common


#client building 
build_client : $(DIRBIN)/tsp_client1_printer

$(DIRBIN)/tsp_client1_printer :  $(LIB_CLIENT) client1.c
	$(COMPILE.c) client1.c -o $(DIRBIN)/client1.o
	$(LINK.c)   $(DIRBIN)/client1.o $(LIB_CLIENT) $(LDLIBS) -o $@

#server building 
build_server : $(DIRBIN)/tsp_server1_sinus

$(DIRBIN)/tsp_server1_sinus:  $(LIB_SERVER) server1.c glue_sserver.c
	$(COMPILE.c) server1.c  -o $(DIRBIN)/server1.o
	$(COMPILE.c) glue_sserver.c  -o $(DIRBIN)/glue_sserver.o
	$(LINK.c)   $(DIRBIN)/server1.o $(DIRBIN)/glue_sserver.o $(LIB_SERVER) $(LDLIBS) -o $@


include $(DEVBASE)/make/Makerules
