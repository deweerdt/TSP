include $(DEVBASE)/make/Makeparams

XINC    = tsp_ringbuf.h  tsp_time.h
#XINC    = tsp_time.h tsp_ringbuf.h  tsp_mtgprof.h

XSCRIPT = 

SOURCES = \
	tsp_time.c \
	tsp_mtgprof.c 

CPPFLAGS += -I.

OBJECTS=$(SOURCES:%.c=%.o) 

TARGET=$(DIRLIB)/libtsp_services.a


# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))
	$(MAKE) export_include

destroy::
	$(RM) 	$(TARGET) $(OBJECTS)
	$(MAKE) remove_include


include $(DEVBASE)/make/Makerules
