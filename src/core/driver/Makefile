include $(DEVBASE)/make/Makeparams.$(T)

XINC    = tsp_consumer.h  tsp_group.h tsp_group_data.h \
          tsp_stream_receiver.h tsp_data_receiver.h tsp_sample_ringbuf.h 

XSCRIPT = 


SOURCES = \
	tsp_consumer.c \
    tsp_group.c \
    tsp_stream_receiver.c \
    tsp_data_receiver.c  \
    tsp_sample_ringbuf.c 


OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_consumer.a

.PRECIOUS: $(TARGET)

all::
	$(MAKE) common debug
common:
	$(MAKE) T=consumer$(D)	build

clean::
	$(MAKE) 	destroy

debug::
	$(MAKE) D=.debug	common

build : $(TARGET)($(OBJECTS))


destroy :
	$(RM) 	 $(TARGET) 

print:
	@for file in Makefile $(INCLUDES) global_type.x DESC.GLOBAL $(SOURCES); \
	 do \
		echo "printing $$file..."; \
		il $$file; \
	 done

include $(DEVBASE)/make/Makerules
