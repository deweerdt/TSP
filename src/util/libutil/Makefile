include $(DEVBASE)/make/Makeparams

RPC=NONE

XINC    = tsp_hash.h


SOURCES = \
	tsp_hash.c

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

TARGET1=$(DIRLIB)/libtsp_util.a
TARGET2=$(DIRBIN)/tsp_hash_dump
TARGET3=$(DIRBIN)/tsp_check_host_and_ip

TARGETS=$(TARGET2) $(TARGET3)

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

ifneq (${TSP_TARGET},vxworks)
build:: $(TARGETS) $(TARGET1)($(OBJECTS))
	$(MAKE) export_include

$(TARGET2): tsp_hash.c
	@echo "@@@ Specific Linking  $(TARGET2) ..."
	$(LINK.c) -DMAIN $< -o $(TARGET2)

destroy:: remove_include
	$(RM) $(TARGETS) $(OBJECTS)
endif

include $(DEVBASE)/make/Makerules




