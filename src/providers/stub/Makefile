include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	glue_stub.c \
	server_main.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o)

ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_calc.a

ifeq (${TSP_TARGET},vxworks)
#TARGET= nothing to do
else
TARGET=$(DIRBIN)/tsp_stub_server
endif


CPPFLAGS += -I../../util/libcalc

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::	$(TARGET)

$(TARGET): $(OBJECTS)
	@echo "@@@ Specific Linking $@ ..."
	$(LINK.c) -o $@ $(OBJECTS) $(ADD_LIB) $(LDLIBS)

destroy::
	$(RM) 	 $(TARGET) $(OBJECTS)
 
include $(DEVBASE)/make/Makerules

