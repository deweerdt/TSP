include $(DEVBASE)/make/Makeparams.$(T)

XINC    = tsp_consumer.h  tsp_group.h tsp_group_data.h \
          tsp_stream_receiver.h tsp_data_receiver.h tsp_sample_ringbuf.h 

XSCRIPT = 


SOURCES = \
	tsp_consumer.c \
    tsp_group.c \
    tsp_stream_receiver.c \
    tsp_data_receiver.c  



OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_consumer.a

.PRECIOUS: $(TARGET)

all::
	$(MAKE) common debug
common:
	$(MAKE) T=consumer$(D)	build

debug::
	$(MAKE) D=.debug	common

build : $(TARGET)($(OBJECTS))





clean::
	$(MAKE) clean_common clean_debug

clean_debug:
	$(MAKE) D=.debug clean_common

clean_common:
	$(MAKE) T=provider$(D) destroy

destroy :
	$(RM) 	 $(TARGET) 





include $(DEVBASE)/make/Makerules

