include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------

SOURCES = \
	interface.c \
	support.c \
	gdispmain.c  \
	config.c \
	plotwindow.c \
	animation.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o) 

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_services.a $(DIRLIB)/libtsp_page_config.a

ifeq (${BUILD_CONSUMER},yes)
    TARGET=$(DIRBIN)/tsp_gdisp
endif

#CPPFLAGS += -D_REENTRANT -I. -I$(TSP_INCLIB) -I$(DIRINC) 
CPPFLAGS += `gtk-config --cflags gtk`  
CPPFLAGS += -I../../util/libpages
LDFLAGS += `gtk-config --libs ` 
LDFLAGS += `xml2-config --libs ` 
 

# ---------------------
# How to build
# ---------------------

build::$(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK.c) -o $@ $(OBJECTS) $(ADD_LIB) $(LDLIBS)

destroy ::
	$(RM)	$(TARGET) $(OBJECTS)

include $(DEVBASE)/make/Makerules

