include $(DEVBASE)/make/Makeparams

# -----------------------------------------------------------
XINC    = histogram.h

SOURCES = \
	histogram.c 

OBJECTS=$(SOURCES:%.c=%.o) 

TARGET=$(DIRLIB)/libtsp_histo.a

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	 $(TARGET) 

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules



