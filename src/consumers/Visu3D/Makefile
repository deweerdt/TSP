# -*- Makefile -*-
include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
SOURCES = \
	Loader3ds.c \
	glScene.c

ifeq (${USABLE_OPENGLUT},yes) 
	SOURCES += testTSP.c 
endif


OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o)

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a  $(DIRLIB)/libtsp_services.a
LDLIBS +=  -L/usr/X11R6/lib  -L/usr/lib -lm -lGLU -lGL -lglut -lX11 -Llib -lXmu

ifeq (${BUILD_CONSUMER},yes)
    TARGET1=$(DIRBIN)/Visu3D
    TARGETS=$(TARGET1)
endif


CPPFLAGS += -I. -DYY_NO_UNPUT 
#CPPFLAGS += -I. -I$(HOME)/TSP/glut-3.7/include -DYY_NO_UNPUT 

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGETS) 

$(TARGET1): $(ADD_LIB) $(OBJECTS)
	@echo "@@@ Specific Linking $< ..."
#	$(LINK.c) $< -o $@ $(OBJECTS) $(HOME)/TSP/glut-3.7/lib/glut/libglut.a  -L$(DIRLIB) $(ADD_LIB) $(LDLIBS)
	$(LINK.c) $< -o $@ $(OBJECTS) -L$(DIRLIB) $(ADD_LIB) $(LDLIBS)

destroy::
	$(RM) $(TARGETS) $(OBJECTS)

include $(DEVBASE)/make/Makerules

