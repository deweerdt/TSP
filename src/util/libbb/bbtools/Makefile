include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	bb_checkid.c \
	bb_dump.c \
	bb_destroy.c \
	bb_read.c \
	bb_write.c \
	bb_test.c 

OBJECTS = $(SOURCES:%.c=$(DIRBIN)/%.o)

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
TARGETS=$(TARGET0) $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5)
endif

CPPFLAGS += -I..

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::	$(TARGETS) 

$(TARGET1): $(DIRLIB)/libbb.a

$(TARGET2): $(DIRLIB)/libbb.a

$(TARGET3): $(DIRLIB)/libbb.a

$(TARGET4): $(DIRLIB)/libbb.a

destroy ::
	$(RM) $(TARGETS)

include $(DEVBASE)/make/Makerules

