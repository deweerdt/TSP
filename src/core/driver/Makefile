include $(DEVBASE)/make/Makeparams

XINC    = tsp_consumer.h  
#XINC    = tsp_consumer.h  tsp_group.h tsp_group_data.h \
          tsp_stream_receiver.h tsp_data_receiver.h tsp_sample_ringbuf.h 

XSCRIPT = 


SOURCES = \
	tsp_consumer.c \
    tsp_group.c \
    tsp_stream_receiver.c \
    tsp_data_receiver.c  

OBJECTS=$(SOURCES:%.c=%.o) 

TARGET=$(DIRLIB)/libtsp_consumer.a

CPPFLAGS += -I../rpc -I../misc_utils

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))
	$(MAKE) export_include

destroy::
	$(RM) 	 $(TARGET) 
	$(MAKE) remove_include





include $(DEVBASE)/make/Makerules

