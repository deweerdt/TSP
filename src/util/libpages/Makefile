include $(DEVBASE)/make/Makeparams

# -----------------------------------------------------------

XINC	= page_config.h

SOURCES = page_config.c

OBJECTS	=$(SOURCES:%.c=%.o) 

TARGET	=$(DIRLIB)/libtsp_page_config.a

CPPFLAGS +=   `xml2-config --cflags`

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

ifneq (${TSP_TARGET},vxworks)
build:: $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	 $(TARGET) $(OBJECTS)
endif

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules



