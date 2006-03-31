include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	client_group.c

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a  $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_calc.a $(DIRLIB)/libtsp_common.a

ifeq (${BUILD_CONSUMER},yes)
    TARGET=$(DIRBIN)/tsp_testgrp_client
endif

# not very nice, but allow to use tsp_group_dta.h for direct checks
CPPFLAGS += -I../../core/driver -I../../core/rpc -I../../core/misc_utils

# ---------------------
# How to build
# ---------------------

build:: $(TARGET) 

$(TARGET): $(SOURCES)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB) $(LDLIBS)

destroy::
	$(RM) 	 $(TARGET) *.o

include $(DEVBASE)/make/Makerules

