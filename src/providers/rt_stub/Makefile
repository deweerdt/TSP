include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	stub_loopOnRtc.c

ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_calc.a $(DIRLIB)/libtsp_histo.a

TARGET=$(DIRBIN)/tsp_rtc_server

CPPFLAGS += -I../../util/libcalc -I../../util/libhisto

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET) 

$(TARGET): $(SOURCES)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB) $(LDLIBS)

destroy::
	$(RM) 	 $(TARGET) *.o

include $(DEVBASE)/make/Makerules

