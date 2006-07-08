include $(DEVBASE)/make/Makeparams

RPC=NONE

XINC    = tsp_hash.h


SOURCES1 = \
	tsp_hash.c


SOURCES2 = \
	tsp_hash.c \
	tsp_hash_main.c

OBJECTS1=$(SOURCES1:%.c=$(DIROBJ)/%.o) 
OBJECTS2=$(SOURCES2:%.c=$(DIROBJ)/%.o) 

TARGET1=$(DIRLIB)/libtsp_util.a
TARGET2=$(DIRBIN)/tsp_hash_dump
TARGET3=$(DIRBIN)/tsp_check_host_and_ip

TARGETS=$(TARGET1) $(TARGET2) $(TARGET3) 

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

ifneq (${TSP_TARGET},vxworks)
build:: $(TARGET3) $(TARGET2)($(OBJECTS2)) $(TARGET1)($(OBJECTS1))
	$(MAKE) export_include


destroy:: remove_include
	$(RM) $(TARGETS) $(OBJECTS)
endif

include $(DEVBASE)/make/Makerules




