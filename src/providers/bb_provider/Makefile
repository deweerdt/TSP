include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	bb_tsp_provider.c \
	bb_tsp_provider_main.c

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libbb.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_calc.a 

ifeq (${TSP_TARGET},vxworks)
#TARGET= nothing to do
else
TARGET=$(DIRBIN)/bb_tsp_provider
endif


CPPFLAGS += -I../../util/libbb

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::	$(TARGET) 

$(TARGET) : $(OBJECTS)
	@echo "@@@ Specific Linking $< ..."
	$(LINK.c) $(OBJECTS) -o $@ -L$(DIRLIB) $(ADD_LIB) $(LDLIBS)

destroy::
	$(RM) $(TARGET) $(OBJECTS)

include $(DEVBASE)/make/Makerules

