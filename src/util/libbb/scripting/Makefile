include $(DEVBASE)/make/Makeparams

# -----------------------------------------------------------

SOURCES = \
	bb_utils.c \
	bb_core.c \
	bb_simple.c \
	bb_sha1.c \
	bb_tools.c \
	bb_tools_tclwrap.c

SHOBJECTS=$(SOURCES:%.c=$(DIRSHOBJ)/%.o) 

SCRIPT_SHLIB_TARGET=libbb

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

wrappergen : bb_tools.i
	-@swig -tcl -namespace  -pkgversion "0.1" -o bb_tools_tclwrap.c -I.. bb_tools.i


bb_tools_tclwrap.c : wrappergen

build:: wrappergen $(SHOBJECTS)
	$(MAKE) make_shlib

destroy::
	$(RM) $(SHOBJECTS)
	$(MAKE) remove_shlib

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules


