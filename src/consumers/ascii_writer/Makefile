# -*- Makefile -*-
include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	tsp_ascii_writer_config_yy.c  \
	tsp_ascii_writer_config.tab.c \
	tsp_ascii_writer.c \
	tsp_ascii_writer_main.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o)

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a  $(DIRLIB)/libtsp_services.a
LDLIBS += -lfl

ifeq (${BUILD_CONSUMER},yes)
    TARGET1=$(DIRBIN)/tsp_ascii_writer
    TARGET2=$(DIRBIN)/tsp_ascii_writer_config_analyse
    TARGETS=$(TARGET1)
endif


CPPFLAGS += -I.

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGETS) 

tsp_ascii_writer_config_yy.c :  tsp_ascii_writer_config.lex
	flex -o$@ $<

tsp_ascii_writer_config.tab.c : tsp_ascii_writer_config.y
	bison --debug -v -d $<

$(TARGET1): $(ADD_LIB) $(OBJECTS)
	@echo "@@@ Specific Linking $< ..."
	$(LINK.c) $< -o $@ $(OBJECTS) -L$(DIRLIB) $(ADD_LIB) $(LDLIBS)

$(TARGET2): $(ADD_LIB) $(OBJECTS)
	@echo "@@@ Specific Linking $< ..."
	$(LINK.c) $< -o $@ $(OBJECTS) -L$(DIRLIB) $(ADD_LIB) $(LDLIBS)

destroy::
	$(RM) $(TARGETS) $(OBJECTS)

include $(DEVBASE)/make/Makerules

