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
	gdisp_animatedLogo.c   \
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
	gdisp_splashScreen.c   \
	gdisp_pixmaps.c

PLUGINS_SOURCES = \
	gdisp_utils.c \
	gdisp_toString.c \
	gdisp_xml.c \
	gdisp_format.c \
	gdisp_popupMenu.c \
	gdisp_plot2D.c \
	gdisp_plotText.c 
ifeq (${USABLE_OPENGL},yes)
PLUGINS_SOURCES += \
	gdisp_plotOrbital.c \
	gdisp_openglUtils.c
endif

OBJECTS = $(SOURCES:%.c=$(DIROBJ)/%.o) 
ifeq (${SHAREDLIB_ENABLED},yes)
SHOBJECTS = $(SOURCES:%.c=$(DIRSHOBJ)/%.o) 
else
OBJECTS += $(PLUGINS_SOURCES:%.c=$(DIROBJ)/%.o) 
endif

ADD_LIB =  $(DIRLIB)/libtsp_consumer.a \
	   $(DIRLIB)/libtsp_services.a \
	   $(DIRLIB)/libtsp_util.a \
	   $(DIRLIB)/libgtk_1.2_utils.a

TARGET=$(DIRBIN)/tsp_gdisp+

CPPFLAGS += `gtk-config --cflags gtk`  
CPPFLAGS += `xml2-config --cflags`

# in order to avoid having GTK stuffs in ${HOME_EXEC_CURRENT)/include.
CPPFLAGS += -I$(DEVBASE)/external/gtk-1.2
#

LDFLAGS  += `gtk-config --libs ` 
LDFLAGS  += `xml2-config --libs ` 

ifeq (${SHAREDLIB_ENABLED},yes)

# ---------------------------------------------------------
# How to build utility shared library.
# ---------------------------------------------------------

UTIL_TARGET = $(DIRLIB)/libtsp_gdispUtils.so

$(UTIL_TARGET): gdisp_utils.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_utils.o gdisp_utils.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_toString.o gdisp_toString.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_xml.o gdisp_xml.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_format.o gdisp_format.c
	$(LINK.c) -shared -o $@ $(DIRSHOBJ)/gdisp_utils.o $(DIRSHOBJ)/gdisp_toString.o $(DIRSHOBJ)/gdisp_xml.o $(DIRSHOBJ)/gdisp_format.o

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
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_plotText.o gdisp_plotText.c
	$(LINK.c) -shared -o $@ $(DIRSHOBJ)/gdisp_plotText.o

# ---------------------------------------------------------
# How to build orbital plot shared library.
# ---------------------------------------------------------

PLOTORBITAL_TARGET = $(DIRLIB)/libtsp_gdispPlotOrbital.so

OGL = -lGL -lGLU

$(PLOTORBITAL_TARGET): gdisp_plotOrbital.c gdisp_openglUtils.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_plotOrbital.o gdisp_plotOrbital.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_openglUtils.o gdisp_openglUtils.c
	$(LINK.c) -shared -o $@ $(DIRSHOBJ)/gdisp_plotOrbital.o gdisp_openglUtils.o $(OGL)

# ---------------------------------------------------------
# How to build main program.
# ---------------------------------------------------------

SECONDARY_TARGETS = $(UTIL_TARGET) $(PLOT2D_TARGET) $(PLOTTEXT_TARGET)
ifeq (${USABLE_OPENGL},yes)
SECONDARY_TARGETS += $(PLOTORBITAL_TARGET)
endif

endif

build::$(TARGET)

ifeq (${USABLE_OPENGL},yes)
LDLIBS += -lGL -lGLU
endif

ifeq (${SHAREDLIB_ENABLED},yes)
$(TARGET): $(SECONDARY_TARGETS) $(OBJECTS)
	$(LINK.c) -o $@ $(OBJECTS) $(SECONDARY_TARGETS) $(ADD_LIB) $(LDLIBS)
else
$(TARGET): $(OBJECTS)
	$(LINK.c) -o $@ $(OBJECTS) $(ADD_LIB) $(LDLIBS)
endif

destroy ::
	$(RM) $(TARGET) $(OBJECTS) $(SECONDARY_TARGETS) *.o *~

include $(DEVBASE)/make/Makerules

