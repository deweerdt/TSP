/*

$Id: gdisp_plotEarth.h,v 1.2 2007-11-21 21:55:35 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2007 - Euskadi.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer: tsp@astrium-space.com
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Definition of 'Earth plot' private structures.

-----------------------------------------------------------------------
*/

#ifndef __EARTH_PLOT_H__
#define __EARTH_PLOT_H__

/*
 * Private structure of a 'Earth plot'.
 */
typedef struct Earth_T_ {

  /*
   * Attributes.
   */
  gboolean             ethHasFocus;

  /*
   * Graphic widget.
   */
  GtkWidget           *ethArea;
  guint                ethAreaWidth;
  guint                ethAreaHeight;

  GdkGC               *ethGContext;
  GdkFont             *ethFont;
  GdkPixmap           *ethBackBuffer;
  gboolean             ethBackBufferIsDirty;

  /*
   * Parent widget.
   */
  GtkWidget           *ethParent;

  /*
   * Popup menus.
   */
  PopupMenu_T         *ethMainMenu;

  /*
   * Data for Earth and Frontier plotting.
   */
  gfloat              *ethEarthBoundingBox;
  guint                ethNbEarthZones;
  guint               *ethNbPointsPerEarthZones;
  gfloat              *ethEarthZoneLonLatCoordinates;
  guint                ethNbFrontierZones;
  guint               *ethNbPointsPerFrontierZones;
  gfloat              *ethFrontierZoneLonLatCoordinates;

} Earth_T;

/*
 * Prototype.
 */
void
gdisp_getEarthData ( float        **earthBoundingBox,
		     unsigned int  *nbEarthZones,
		     unsigned int **nbPointsPerEarthZones,
		     float        **earthZoneLonLatCoordinates,
		     unsigned int  *nbFrontierZones,
		     unsigned int **nbPointsPerFrontierZones,
		     float        **frontierZoneLonLatCoordinates );


#endif /* __EARTH_PLOT_H__ */
