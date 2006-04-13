include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
XSCRIPT = tsp_request_open \
	  tsp_request_close \
	tsp_request_async_sample_read \
	 tsp_request_async_sample_write \
	 tsp_request_information \
	 tsp_request_filtered_information \
	 tsp_request_extended_information
SOURCES = \
	generic_consumer.c \
	generic_consumer_main.c

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_common.a

ifeq (${BUILD_CONSUMER},yes)
	TARGET=$(DIRBIN)/tsp_request_generic
endif

# ---------------------
# How to build
# ---------------------
build :: $(TARGET) 
	$(MAKE) export_script

$(TARGET): $(DIROBJ)/generic_consumer.o $(DIROBJ)/generic_consumer_main.o $(ADD_LIB)
	@echo "@@@ Specific Linking  $(TARGET) ..."
	$(LINK.c) -o $(TARGET) $(DIROBJ)/generic_consumer.o $(DIROBJ)/generic_consumer_main.o -L$(DIRLIB) $(ADD_LIB) $(LDLIBS)

destroy ::
	$(RM) $(TARGET) $(DIROBJ)/generic_consumer.o $(DIROBJ)/generic_consumer_main.o
	$(RM) $(TARGET)

include $(DEVBASE)/make/Makerules

