include $(DEVBASE)/make/Makeparams

XINC    = tsp_provider_init.h

SOURCES = \
	tsp_provider_init.c

OBJECTS=$(SOURCES:%.c=%.o) 

TARGET=$(DIRLIB)/libtsp_provider.a

CPPFLAGS += -I../rpc -I../ctrl

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))
	$(MAKE) export_include

destroy::
	$(RM) 	$(TARGET)
	$(MAKE) remove_include


include $(DEVBASE)/make/Makerules
