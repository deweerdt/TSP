include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	client_callback_stdout.c

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_calc.a

ADD_INCLUDE = -I $(DEVBASE)/src/util/libcalc

ifeq (${BUILD_CONSUMER},yes)
    TARGET=$(DIRBIN)/tsp_stdout_client_callback
endif

# ---------------------
# How to build
# ---------------------
build :: $(TARGET) 

$(TARGET): $(SOURCES)
	$(LINK.c) -o $@ $(ADD_INCLUDE) $(SOURCES) $(ADD_LIB) $(LDLIBS)

destroy ::
	$(RM)	$(TARGET) *.o

include $(DEVBASE)/make/Makerules

