include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------

SOURCES = \
	gdisp_main.c        \
	gdisp_kernel.c      \
	gdisp_mainBoard.c   \
	gdisp_providers.c   \
	gdisp_consumers.c   \
	gdisp_symbols.c     \
	gdisp_pages.c       \
	gdisp_sampling.c    \
	gdisp_colormap.c    \
	gdisp_defaultPlot.c \
	gdisp_plot2D.c      \
	gdisp_pointArray.c  \
	gdisp_utils.c

OBJECTS = $(SOURCES:%.c=%.o) 

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_services.a

TARGET=$(DIRBIN)/tsp_gdisp+

CPPFLAGS += `gtk-config --cflags gtk`  
LDFLAGS  += `gtk-config --libs ` 
LDFLAGS  += `xml2-config --libs ` 

# ---------------------
# How to build
# ---------------------

build::$(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK.c) -o $@ $(OBJECTS) $(ADD_LIB) $(LDLIBS)

destroy ::
	$(RM)	$(TARGET) $(OBJECTS)

include $(DEVBASE)/make/Makerules

