include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
CC = gcc
SOURCES = \
	gdisp_main.c           \
	gdisp_kernel.c         \
	gdisp_preferences.c    \
	gdisp_configuration.c  \
	gdisp_mainBoard.c      \
	gdisp_pilotBoard.c     \
	gdisp_dataBook.c       \
	gdisp_hosts.c          \
	gdisp_providers.c      \
	gdisp_consumers.c      \
	gdisp_symbols.c        \
	gdisp_sampledSymbols.c \
	gdisp_graphics.c       \
	gdisp_pages.c          \
	gdisp_sampling.c       \
	gdisp_colormap.c       \
	gdisp_defaultPlot.c    \
	gdisp_pointArray.c     \
	gdisp_pixmaps.c

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o) 
SHOBJECTS = $(SOURCES:%.c=$(DIRSHOBJ)/%.o) 

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a $(DIRLIB)/libtsp_services.a

TARGET=$(DIRBIN)/tsp_gdisp+

CPPFLAGS += `gtk-config --cflags gtk`  
CPPFLAGS += `xml2-config --cflags`
LDFLAGS  += `gtk-config --libs ` 
LDFLAGS  += `xml2-config --libs ` 

# ---------------------------------------------------------
# How to build utility shared library.
# ---------------------------------------------------------

UTIL_TARGET = $(DIRLIB)/libtsp_gdispUtils.so

$(UTIL_TARGET): gdisp_utils.c
	$(COMPILE.c) -fPIC -o gdisp_utils.o gdisp_utils.c
	$(COMPILE.c) -fPIC -o gdisp_toString.o gdisp_toString.c
	$(COMPILE.c) -fPIC -o gdisp_xml.o gdisp_xml.c
	$(LINK.c) -shared -o $@ gdisp_utils.o gdisp_toString.o gdisp_xml.o

# ---------------------------------------------------------
# How to build plot 2D shared library.
# ---------------------------------------------------------

PLOT2D_TARGET = $(DIRLIB)/libtsp_gdispPlot2D.so

$(PLOT2D_TARGET): gdisp_plot2D.c
	$(COMPILE.c) -fPIC -o gdisp_plot2D.o gdisp_plot2D.c
	$(LINK.c) -shared -o $@ gdisp_plot2D.o

# ---------------------------------------------------------
# How to build plot text shared library.
# ---------------------------------------------------------

PLOTTEXT_TARGET = $(DIRLIB)/libtsp_gdispPlotText.so

$(PLOTTEXT_TARGET): gdisp_plotText.c
	$(COMPILE.c) -fPIC -o gdisp_plotText.o gdisp_plotText.c
	$(LINK.c) -shared -o $@ gdisp_plotText.o

# ---------------------------------------------------------
# How to build orbital plot shared library.
# ---------------------------------------------------------

PLOTORBITAL_TARGET = $(DIRLIB)/libtsp_gdispPlotOrbital.so

OGL = -lGL -lGLU

$(PLOTORBITAL_TARGET): gdisp_plotOrbital.c gdisp_openglUtils.c
	$(COMPILE.c) -fPIC -o gdisp_plotOrbital.o gdisp_plotOrbital.c
	$(COMPILE.c) -fPIC -o gdisp_openglUtils.o gdisp_openglUtils.c
	$(LINK.c) -shared -o $@ gdisp_plotOrbital.o gdisp_openglUtils.o $(OGL)

# ---------------------------------------------------------
# How to build main program.
# ---------------------------------------------------------

SECONDARY_TARGETS = $(UTIL_TARGET) $(PLOT2D_TARGET) $(PLOTTEXT_TARGET)
ifeq (${USABLE_OPENGL},yes)
SECONDARY_TARGETS += $(PLOTORBITAL_TARGET)
endif

build::$(TARGET)

$(TARGET): $(SECONDARY_TARGETS) $(OBJECTS)
	$(LINK.c) -o $@ $(OBJECTS) $(SECONDARY_TARGETS) $(ADD_LIB) $(LDLIBS)

destroy ::
	$(RM) $(TARGET) $(OBJECTS) $(SECONDARY_TARGETS) *.o *~

include $(DEVBASE)/make/Makerules

