include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	client_res.c

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a  $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_res.a

TARGET=$(DIRBIN)/tsp_res_writer

CPPFLAGS += -I../../util/libres

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

