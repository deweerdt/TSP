SET(LIBGTK1.2_SRC
gtk/gtkbordercombo.c
gtk/gtkcharsel.c
gtk/gtkcheckitem.c
gtk/gtkcolorcombo.c
gtk/gtkcombobox.c
gtk/gtkdatabox.c
gtk/gtkdirtree.c
gtk/gtkextra.c
gtk/gtkfilelist.c
gtk/gtkfontcombo.c
gtk/gtkiconfilesel.c
gtk/gtkiconlist.c
gtk/gtkitementry.c
gtk/gtkplot3d.c
gtk/gtkplotbar.c
gtk/gtkplotbox.c
gtk/gtkplot.c
gtk/gtkplotcanvas.c
gtk/gtkplotcsurface.c
gtk/gtkplotdata.c
gtk/gtkplotdt.c
gtk/gtkplotflux.c
gtk/gtkplotgdk.c
gtk/gtkplotpc.c
gtk/gtkplotpixmap.c
gtk/gtkplotpolar.c
gtk/gtkplotprint.c
gtk/gtkplotps.c
gtk/gtkplotsurface.c
gtk/gtkpsfont.c
gtk/gtksheet.c
gtk/gtktogglecombo.c)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/gtk-1.2/)

ADD_LIBRARY(gtk_1.2_utils ${LIBGTK1.2_SRC})

ADD_DEFINITIONS(${GTK_CFLAGS})

TARGET_LINK_LIBRARIES(gtk_1.2_utils ${GTK_LIBRARY} pthread)
TSP_LIBRARY_INSTALL(gtk_1.2_utils) 
