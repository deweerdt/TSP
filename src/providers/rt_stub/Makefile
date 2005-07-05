include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	rtc_util.c \
	bb_vars.c \
	main_stub.c

ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_calc.a $(DIRLIB)/libtsp_histo.a $(DIRLIB)/libbb.a

TARGET1=$(DIRBIN)/bb_rt_stub 
TARGET2=$(DIRBIN)/tsp_rt_stub
TARGET=$(TARGET1) $(TARGET2)

CPPFLAGS += -I../../util/libcalc -I../../util/libhisto

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET) 

$(TARGET1): $(SOURCES)
	$(LINK.c) -o $@ $(SOURCES) $(ADD_LIB) $(LDLIBS)

$(TARGET2): stub_loopOnRtc.c
	$(LINK.c) -o $@ $< $(ADD_LIB) $(LDLIBS)
destroy::
	$(RM) 	 $(TARGET) *.o

include $(DEVBASE)/make/Makerules

