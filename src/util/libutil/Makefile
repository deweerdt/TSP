include $(DEVBASE)/make/Makeparams

RPC=NONE

XINC    = hash.h


SOURCES = \
	hash.c

OBJECTS=$(SOURCES:%.c=%.o) 

TARGET=$(DIRLIB)/libtsp_util.a
EXETARGET=


# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

ifneq (${TSP_TARGET},vxworks)
build:: $(TARGET)($(OBJECTS)) $(DIRBIN)/hash_dump export_include

$(DIRBIN)/hash_dump: hash.c
	cc -DMAIN $< -o $@

destroy:: remove_include
	$(RM) 	 $(EXETARGET) $(TARGET) $(OBJECTS)
endif

include $(DEVBASE)/make/Makerules




