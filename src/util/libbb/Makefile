include $(DEVBASE)/make/Makeparams

SUBDIRS = bbtools
ifeq (${BUILD_SCRIPT_BINDING},yes)
	SUBDIRS += scripting 
endif

# -----------------------------------------------------------
XINC    = bb_utils.h bb_core.h bb_simple.h bb_sha1.h \
	  bb_tools.h

SOURCES = \
	bb_utils.c \
	bb_alias.c \
	bb_core.c \
	bb_simple.c \
	bb_sha1.c \
	bb_tools.c

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o)
ifeq (${BUILD_SCRIPT_BINDING},yes)
SHOBJECTS=$(SOURCES:%.c=$(DIRSHOBJ)/%.o)
endif
TARGET=$(DIRLIB)/libbb.a

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS)) $(SHOBJECTS)
	$(MAKE) export_include
destroy::
	$(RM) 	 $(TARGET) $(SHOBJECTS)
	$(MAKE) remove_include	

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules
include $(DEVBASE)/make/Makesubdirs


