include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
XSCRIPT = bb_findlabel
SOURCES = \
	bb_checkid.c \
	bb_dump.c \
	bb_destroy.c \
	bb_read.c \
	bb_write.c \
	bb_test.c \
	bb_tools_main.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o)

ADD_LIB =  $(DIRLIB)/libbb.a

ifeq (${TSP_TARGET},vxworks)
#TARGET= nothing to do
else
TARGET0=$(DIRBIN)/bb_checkid
TARGET1=$(DIRBIN)/bb_dump
TARGET2=$(DIRBIN)/bb_destroy
TARGET3=$(DIRBIN)/bb_read
TARGET4=$(DIRBIN)/bb_write
TARGET5=$(DIRBIN)/bb_test
TARGET6=$(DIRBIN)/bb_tools
TARGETS=$(TARGET0) $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6)
endif

CPPFLAGS += -I..

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::	$(TARGETS) 
	$(MAKE) export_script

$(TARGET1): $(DIRLIB)/libbb.a 

$(TARGET2): $(DIRLIB)/libbb.a

$(TARGET3): $(DIRLIB)/libbb.a

$(TARGET4): $(DIRLIB)/libbb.a

$(TARGET5): $(DIRLIB)/libbb.a

$(TARGET6): $(DIRLIB)/libbb.a $(DIROBJ)/bb_tools_main.o
	@echo "@@@ Specific Linking  $(TARGET6) ..."
	$(LINK.c) -o $(TARGET6) $(DIROBJ)/bb_tools_main.o -L$(DIRLIB) $(ADD_LIB) $(LDLIBS) 


destroy ::
	$(RM) $(TARGETS)
	$(MAKE) remove_script

include $(DEVBASE)/make/Makerules

