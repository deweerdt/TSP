include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
CC = gcc
SOURCES = \
	gdisp_main.c           \
	gdisp_kernel.c         \
	gdisp_modules.c        \
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
	gdisp_splashScreen.c

PLUGINS_SOURCES = \
	gdisp_utils.c \
	gdisp_pixmaps.c \
	gdisp_toString.c \
	gdisp_xml.c \
	gdisp_format.c \
	gdisp_dataBox.c \
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
	   $(DIRLIB)/libtsp_util.a  \
	   $(DIRLIB)/libtsp_common.a  \
	   $(DIRLIB)/libgtk_1.2_utils.a

TARGET=$(DIRBIN)/targa

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

UTIL_TARGET = $(DIRLIB)/libtsp_targaUtils.so

$(UTIL_TARGET): gdisp_utils.c gdisp_pixmaps.c gdisp_toString.c gdisp_xml.c gdisp_format.c gdisp_dataBox.c gdisp_popupMenu.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_utils.o gdisp_utils.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_pixmaps.o gdisp_pixmaps.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_toString.o gdisp_toString.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_xml.o gdisp_xml.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_format.o gdisp_format.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_dataBox.o gdisp_dataBox.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_popupMenu.o gdisp_popupMenu.c
	$(LINK.c) -shared -o $@ $(DIRSHOBJ)/gdisp_utils.o $(DIRSHOBJ)/gdisp_toString.o $(DIRSHOBJ)/gdisp_xml.o $(DIRSHOBJ)/gdisp_format.o $(DIRSHOBJ)/gdisp_dataBox.o $(DIRSHOBJ)/gdisp_popupMenu.o $(DIRSHOBJ)/gdisp_pixmaps.o 

# ---------------------------------------------------------
# How to build plot 2D shared library.
# ---------------------------------------------------------

PLOT2D_TARGET = $(DIRLIB)/libtsp_targaPlot2D.so

$(PLOT2D_TARGET): gdisp_plot2D.c
	$(COMPILE.c) -fPIC -o gdisp_plot2D.o gdisp_plot2D.c
	$(LINK.c) -shared -o $@ gdisp_plot2D.o $(UTIL_TARGET)

# ---------------------------------------------------------
# How to build plot text shared library.
# ---------------------------------------------------------

PLOTTEXT_TARGET = $(DIRLIB)/libtsp_targaPlotText.so

$(PLOTTEXT_TARGET): gdisp_plotText.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_plotText.o gdisp_plotText.c
	$(LINK.c) -shared -o $@ $(DIRSHOBJ)/gdisp_plotText.o $(UTIL_TARGET)

# ---------------------------------------------------------
# How to build orbital plot shared library.
# ---------------------------------------------------------

PLOTORBITAL_TARGET = $(DIRLIB)/libtsp_targaPlotOrbital.so

OGL = -lGL -lGLU

$(PLOTORBITAL_TARGET): gdisp_plotOrbital.c gdisp_openglUtils.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_plotOrbital.o gdisp_plotOrbital.c
	$(COMPILE.c) -fPIC -o $(DIRSHOBJ)/gdisp_openglUtils.o gdisp_openglUtils.c
	$(LINK.c) -shared -o $@ $(DIRSHOBJ)/gdisp_plotOrbital.o $(DIRSHOBJ)/gdisp_openglUtils.o $(UTIL_TARGET) $(OGL)

# ---------------------------------------------------------
# How to build main program.
# ---------------------------------------------------------

GRAPHIC_TARGETS = $(PLOT2D_TARGET) $(PLOTTEXT_TARGET)
ifeq (${USABLE_OPENGL},yes)
GRAPHIC_TARGETS += $(PLOTORBITAL_TARGET)
endif

# ---------------------------------------------------------
# Extra definition.
# ---------------------------------------------------------
CPPFLAGS += -DGD_DYNAMIC_GRAPHIC_MODULES

endif

build::$(TARGET)

ifeq (${USABLE_OPENGL},yes)
LDLIBS += -lGL -lGLU
endif

ifeq (${SHAREDLIB_ENABLED},yes)
$(TARGET): $(UTIL_TARGET) $(GRAPHIC_TARGETS) $(OBJECTS)
	$(LINK.c) -o $@ $(OBJECTS) $(UTIL_TARGET) $(ADD_LIB) $(LDLIBS)
else
$(TARGET): $(OBJECTS)
	$(LINK.c) -o $@ $(OBJECTS) $(ADD_LIB) $(LDLIBS)
endif

destroy ::
	$(RM) $(TARGET) $(OBJECTS) $(UTIL_TARGET) $(GRAPHIC_TARGETS) *.o *~
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules

