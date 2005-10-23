include $(DEVBASE)/make/Makeparams

XINC    = tsp_provider.h tsp_datapool.h
#XINC    = tsp_provider.h tsp_session.h   glue_sserver.h tsp_datapool.h \
          tsp_group_algo.h tsp_data_sender.h tsp_group_algo_data.h \
          tsp_stream_sender.h tsp_request.h

SOURCES = \
	tsp_default_glu.c \
	tsp_provider.c \
	tsp_session.c \
	tsp_datapool.c \
	tsp_group_algo.c \
	tsp_data_sender.c \
	tsp_stream_sender.c \
	tsp_request_handler.c \
	tsp_filter_symbol.c


OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

TARGET=$(DIRLIB)/libtsp_provider.a

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
