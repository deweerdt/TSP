include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	client_stdout.c

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_calc.a

ifeq (${BUILD_CONSUMER},yes)
	TARGET=$(DIRBIN)/tsp_stdout_client
endif

# ---------------------
# How to build
# ---------------------
build :: $(TARGET) 

$(TARGET): $(SOURCES)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB) $(LDLIBS)

destroy ::
	$(RM)	$(TARGET) *.o

include $(DEVBASE)/make/Makerules

