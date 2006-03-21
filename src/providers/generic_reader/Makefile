include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------

SOURCES = \
	macsim_fmt.c \
	generic_reader.c \
	generic_reader_main.c

ifeq (${TSP_TARGET},vxworks)
#TARGET = nothing to do 
else
TARGET=$(DIRBIN)/tsp_generic_reader
endif

ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_common.a $(DIRLIB)/libtsp_services.a  $(DIRLIB)/libtsp_res.a


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

