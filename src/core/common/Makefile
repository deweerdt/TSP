include $(DEVBASE)/make/Makeparams

XINC    = tsp_common.h
SOURCES = \
	tsp_common.c 

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

TARGET=$(DIRLIB)/libtsp_common.a

CPPFLAGS += -I../rpc -I ../misc_utils

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: export_include $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	$(TARGET)
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules
