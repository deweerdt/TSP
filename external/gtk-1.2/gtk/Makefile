include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------

# I do not use XINC in order to avoid having
# GTK stuffs in ${HOME_EXEC_CURRENT)/include.

CC        = gcc

SOURCES   = gtkdatabox.c

OBJECTS   = $(SOURCES:%.c=$(DIROBJ)/%.o) 

CPPFLAGS += `gtk-config --cflags gtk` 
CPPFLAGS += -I..

LDFLAGS  += `gtk-config --libs ` 

TARGET    = $(DIRLIB)/libgtk_1.2_utils.a

build:: $(TARGET)($(OBJECTS))

destroy::
	$(RM) $(TARGET) $(OBJECTS) *~

include $(DEVBASE)/make/Makerules
