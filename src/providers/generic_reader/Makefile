include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------

SOURCES = \
	macsim_fmt.c \
	glu_genreader.c \
	generic_reader.c \
	generic_reader_main.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o)

ifeq (${TSP_TARGET},vxworks)
#TARGET = nothing to do 
else
TARGET=$(DIRBIN)/tsp_generic_reader
endif

ADD_LIB =  $(DIRLIB)/libtsp_provider.a $(DIRLIB)/libtsp_common.a $(DIRLIB)/libtsp_services.a 

CPPFLAGS += -Werror

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET) 

$(OBJECTS) : $(ADD_LIB)

$(TARGET): $(OBJECTS) $(ADD_LIB)
	$(LINK.c) -o $@ $(OBJECTS) $(ADD_LIB) $(LDLIBS)


destroy::
	$(RM) $(TARGET) $(OBJECTS) 

include $(DEVBASE)/make/Makerules

