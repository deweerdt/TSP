include $(DEVBASE)/make/Makeparams

SUBDIRS = bbtools

# -----------------------------------------------------------
XINC    = bb_utils.h bb_core.h bb_simple.h bb_sha1.h

SOURCES = \
	bb_utils.c \
	bb_core.c \
	bb_simple.c \
	bb_sha1.c

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

TARGET=$(DIRLIB)/libbb.a

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))
	$(MAKE) export_include
destroy::
	$(RM) 	 $(TARGET) 
	$(MAKE) remove_include	

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules
include $(DEVBASE)/make/Makesubdirs


