include $(DEVBASE)/make/Makeparams

XINC    = tsp_consumer.h

XSCRIPT = 


SOURCES = \
	tsp_consumer.c \
    tsp_group.c \
    tsp_stream_receiver.c \
    tsp_data_receiver.c  

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

TARGET=$(DIRLIB)/libtsp_consumer.a

CPPFLAGS += -I../rpc -I../misc_utils -I../common

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: export_include $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	 $(TARGET) 
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules

