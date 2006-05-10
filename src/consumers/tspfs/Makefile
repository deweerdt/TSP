include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = tspfs.c

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_calc.a $(DIRLIB)/libtsp_common.a
CFLAGS+=-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=25 -Wall -Werror
LDLIBS+=-lfuse

ifeq (${BUILD_CONSUMER},yes)
	TARGET=$(DIRBIN)/tspfs
endif

# ---------------------
# How to build
# ---------------------
build :: $(TARGET) 

$(TARGET): $(SOURCES)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB) $(LDLIBS)

destroy ::
	$(RM) $(TARGET)

include $(DEVBASE)/make/Makerules

