include $(DEVBASE)/make/Makeparams.$(T)





LIB_CLIENT =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_rpc_client.a $(DIRLIB)/libtsp_services.a

#$(LIB_UTIL)

LIB_SERVEUR = $(DIRLIB)/libtsp_rpc_server.a $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_rpc_server.a

#$(LIB_UTIL)

all::
	$(MAKE)  common debug
common: 
	$(MAKE)  T=consumer$(D)	  build_client 
	$(MAKE)  T=provider$(D)   build_server
debug:
	$(MAKE)  D=.debug common


#client building 
build_client : $(DIRBIN)/tsp_client3

$(DIRBIN)/tsp_client3 :  $(LIB_CLIENT) client3.c
	$(COMPILE.c) client3.c -o $(DIRBIN)/client3.o
	$(COMPILE.c) datar.c -o $(DIRBIN)/datar.o
	$(COMPILE.c) dataw.c -o $(DIRBIN)/dataw.o
	$(LINK.c)   $(DIRBIN)/client3.o  $(DIRBIN)/datar.o $(DIRBIN)/dataw.o $(LIB_CLIENT) $(LDLIBS) -o $@

#serveur building 
build_server : $(DIRBIN)/tsp_server3

$(DIRBIN)/tsp_server3 :  $(LIB_SERVEUR) server3.c glue_sserver3.c
	$(COMPILE.c) server3.c  -o $(DIRBIN)/server3.o
	$(COMPILE.c) glue_sserver3.c  -o $(DIRBIN)/glue_sserver3.o
	$(COMPILE.c) datar.c -o $(DIRBIN)/datar.o
	$(COMPILE.c) dataw.c -o $(DIRBIN)/dataw.o
	$(LINK.c)   $(DIRBIN)/server3.o  $(DIRBIN)/datar.o $(DIRBIN)/dataw.o $(DIRBIN)/glue_sserver3.o $(LIB_SERVEUR) $(LDLIBS) -o $@


include $(DEVBASE)/make/Makerules
