include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
XINC    = bb_tsp_provider.h
SOURCES = \
	bb_tsp_provider.c

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

ADD_LIB = \
	$(DIRLIB)/libbb_tsp_provider.a \
	$(DIRLIB)/libtsp_provider.a \
	$(DIRLIB)/libtsp_common.a \
	$(DIRLIB)/libbb.a \
	$(DIRLIB)/libtsp_services.a \
	$(DIRLIB)/libtsp_calc.a 

ifeq (${TSP_TARGET},vxworks)
ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_calc.a  $(DIRLIB)/libbb.a $(DIRLIB)/libtsp_posix.a
TARGET=$(DIRBIN)/bb_vx_provider.o
LDLIBS=
else
TARGET=$(DIRLIB)/libbb_tsp_provider.a
TARGET1=$(DIRBIN)/bb_tsp_provider
TARGETS=$(TARGET1)
endif


CPPFLAGS += -I../../util/libbb

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::	$(TARGET)($(OBJECTS))  $(TARGETS)
	$(MAKE) export_include

$(TARGET1) : $(ADD_LIB) $(DIROBJ)/bb_tsp_provider_main.o
	@echo "@@@ Specific Linking $< ..."
	$(LINK.c) $< -o $@ $(DIROBJ)/bb_tsp_provider_main.o -L$(DIRLIB) $(ADD_LIB)  $(LDLIBS)

destroy::
	$(RM) $(TARGET) $(OBJECTS)
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules

