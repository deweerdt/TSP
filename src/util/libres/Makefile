include $(DEVBASE)/make/Makeparams

RPC=NONE

XINC    = libUTIL.h


SOURCES = \
	use_dbl.c \
	datar.c \
	dataw.c

OBJECTS=$(SOURCES:%.c=%.o) 

TARGET=$(DIRLIB)/libtsp_res.a


# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

ifneq (${TSP_TARGET},vxworks)
build:: $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	 $(TARGET) $(OBJECTS)
endif

include $(DEVBASE)/make/Makerules




