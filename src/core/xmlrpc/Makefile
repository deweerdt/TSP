include $(DEVBASE)/make/Makeparams

XINC = tsp_rpc.h tsp_datastruct.h  # FIXME : shoudl not need to export tsp_datastruct.h
#XINC    = tsp_provider.h glue_sserver.h tsp_datapool.h
#XINC    = tsp_provider.h tsp_session.h   glue_sserver.h tsp_datapool.h \
          tsp_group_algo.h tsp_data_sender.h tsp_group_algo_data.h \
          tsp_stream_sender.h tsp_request.h

SOURCES = \
	tsp_xmlrpc_client.c \
	tsp_xmlrpc_server.c \
	tsp_xmlrpc_util.c


OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

TARGET_C=$(DIRLIB)/libtsp_consumer.a
TARGET_P=$(DIRLIB)/libtsp_provider.a

CPPFLAGS += -I ../misc_utils

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------
BUILD_TARGETS = build_p build_c
ifeq (${BUILD_PROVIDER}, yes)
	BUILD_TARGETS += build_p
endif
ifeq (${BUILD_CONSUMER}, yes)
	BUILD_TARGETS += build_c
endif
		
build:: $(BUILD_TARGETS)

build_p:: export_include $(TARGET_P)($(OBJECTS)) 

build_c:: export_include $(TARGET_C)($(OBJECTS)) 


destroy::
	$(RM) 	$(TARGET)
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules
