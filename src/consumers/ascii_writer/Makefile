# -*- Makefile -*-
include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
XINC    = tsp_ascii_writer.h
SOURCES = \
	tsp_ascii_writer_config_yy.c  \
	y.tab.c \
	tsp_ascii_writer.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o)

ADD_LIB = $(DIRLIB)/libtsp_ascii_writer.a $(DIRLIB)/libtsp_consumer.a  $(DIRLIB)/libtsp_services.a 
LDLIBS += $(LEXLIB)

ifeq (${BUILD_CONSUMER},yes)
    TARGET=$(DIRLIB)/libtsp_ascii_writer.a
    TARGET1=$(DIRBIN)/tsp_ascii_writer
    TARGETS=$(TARGET1)
endif


CPPFLAGS += -I. -DYY_NO_UNPUT

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))  $(TARGETS)
	$(MAKE) export_include

tsp_ascii_writer_config_yy.c :  tsp_ascii_writer_config.lex
	$(LEX) -o$@ $<

y.tab.h : tsp_ascii_writer_config.y
	$(YACC) -d $<

y.tab.c : tsp_ascii_writer_config.y
	$(YACC) -d $<

#bison --debug -v -d $<

$(OBJECTS) : y.tab.h

$(TARGET1): $(ADD_LIB) $(DIROBJ)/tsp_ascii_writer_main.o
	@echo "@@@ Specific Linking $< ..."
	$(LINK.c) $< -o $@ $(DIROBJ)/tsp_ascii_writer_main.o -L$(DIRLIB) $(ADD_LIB) $(LDLIBS)

destroy::
	$(RM) $(TARGETS) $(OBJECTS)
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules

