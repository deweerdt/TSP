include $(DEVBASE)/make/Makeparams
# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	glue_stub.c \
	server_main.c

BBSOURCES = \
	bb_server_main.c

ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_common.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_calc.a

ifeq (${TSP_TARGET},vxworks)
VXTARGET=$(DIRBIN)/tsp_stub_server.o
BBTARGET=$(DIRBIN)/tsp_bb_stub_server.o
else
#VXTARGET= nothing to do
endif

VXOBJECTS=$(SOURCES:%.c=$(DIRBIN)/%.o)
BBOBJECTS=$(BBSOURCES:%.c=$(DIRBIN)/%.o)
VXLOAD=$(SOURCE:%.c=%.o)


CPPFLAGS +=  -I../../util/libcalc

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::$(VXTARGET) $(BBTARGET)


$(VXTARGET): $(SOURCES) $(ADD_LIB)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB)
# $(LDLIBS)

$(BBTARGET): $(BBSOURCES)
	$(LINK.c) -o $@ $(BBSOURCES)

destroy::
	$(RM) $(BBTARGET) $(VXTARGET)  *.o 
 
include $(DEVBASE)/make/Makerules

