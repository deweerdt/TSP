include $(DEVBASE)/make/Makeparams

# -----------------------------------------------------------

SOURCES = \
	../bb_utils.c \
	../bb_core.c \
	../bb_simple.c \
	../bb_sha1.c \
	../bb_tools.c \
	bb_tools_wrap.c

OBJECTS_TEMP=$(SOURCES:../%.c=$(DIRSHOBJ)/%.o) 
OBJECTS=$(OBJECTS_TEMP:%.c=$(DIRSHOBJ)/%.o) 


SCRIPT_SHLIB_TARGET=libbb

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

wrappergen :
	-@swig -tcl -I.. bb_tools.i
	echo $(OBJECTS)

build:: wrappergen $(OBJECTS)
	$(MAKE) make_shlib

destroy::
	$(RM) $(TARGET) $(OBJECTS)
	$(MAKE) remove_shlib

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules


