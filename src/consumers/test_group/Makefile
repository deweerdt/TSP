include $(DEVBASE)/make/Makeparams.$(T)

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	client_group.c

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_rpc_client.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_calc.a

TARGET=$(DIRBIN)/tsp_testgrp_client

# ---------------------
# How to build
# ---------------------
all::
	$(MAKE) T=consumer	build 

build : $(TARGET) 

$(TARGET): $(SOURCES)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB) $(LDLIBS)

clean::
	$(MAKE) T=consumer	destroy

destroy :
	$(RM) 	 $(TARGET) *.o

include $(DEVBASE)/make/Makerules

