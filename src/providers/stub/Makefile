include $(DEVBASE)/make/Makeparams.$(T)

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	glue_stub.c \
	server_main.c

ADD_LIB =  $(DIRLIB)/libtsp_provider_init.a $(DIRLIB)/libtsp_rpc_server.a  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_calc.a

TARGET=$(DIRBIN)/tsp_stub_server

# ---------------------
# How to build
# ---------------------
all::
	$(MAKE) T=provider	build 

build : $(TARGET) 

$(TARGET): $(SOURCES)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB) $(LDLIBS)

clean::
	$(MAKE) T=provider destroy

destroy :
	$(RM) 	 $(TARGET) 

include $(DEVBASE)/make/Makerules

