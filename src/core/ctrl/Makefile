include $(DEVBASE)/make/Makeparams.$(T)

XINC    = tsp_provider.h tsp_session.h   glue_sserver.h tsp_datapool.h \
          tsp_group_algo.h tsp_data_sender.h tsp_group_algo_data.h \
          tsp_stream_sender.h 

SOURCES = \
	tsp_provider.c \
	tsp_session.c \
	glue_sserver.c \
	tsp_datapool.c \
    tsp_group_algo.c \
    tsp_data_sender.c \
    tsp_stream_sender.c 

OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_provider.a

.PRECIOUS: $(TARGET)

all::
	$(MAKE) common debug

clean::
	$(MAKE) destroy
common:
	$(MAKE) T=provider$(D) build
debug::
	$(MAKE) D=.debug	common

build : $(TARGET)($(OBJECTS))



destroy :
	$(RM) 	$(TARGET) 




include $(DEVBASE)/make/Makerules
