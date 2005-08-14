include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
XSCRIPT = bb_findlabel bb_find \
	bb_dump \
	bb_read bb_write \
	bb_check_id bb_destroy \
	bb_synchro_send bb_synchro_recv \
	bb_memset bb_check_version
SOURCES = \
	bb_simu.c \
	bb_tools_main.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o)

ADD_LIB =  $(DIRLIB)/libbb.a

ifeq (${TSP_TARGET},vxworks)
#TARGET= nothing to do
else
TARGET1=$(DIRBIN)/bb_simu
TARGET2=$(DIRBIN)/bb_tools
TARGETS=$(TARGET1) $(TARGET2)
endif

CPPFLAGS += -I..

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::	$(TARGETS) 
	$(MAKE) export_script

$(TARGET1): $(DIRLIB)/libbb.a 

$(TARGET2): $(DIRLIB)/libbb.a $(DIROBJ)/bb_tools_main.o
	@echo "@@@ Specific Linking  $(TARGET2) ..."
	$(LINK.c) -o $(TARGET2) $(DIROBJ)/bb_tools_main.o -L$(DIRLIB) $(ADD_LIB) $(LDLIBS) 


destroy ::
	$(RM) $(TARGETS) $(DIROBJ)/bb_tools_main.o
	$(MAKE) remove_script

include $(DEVBASE)/make/Makerules

