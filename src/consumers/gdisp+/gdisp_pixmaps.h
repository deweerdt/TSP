/*!  \file 

$Id: gdisp_pixmaps.h,v 1.1 2004-10-28 19:15:05 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi.

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

File      : Pixmaps definitions.

-----------------------------------------------------------------------
*/

#ifndef __PIXMAPS_H__
#define __PIXMAPS_H__

/*
 * Pixmap identity definitions.
 */
typedef enum {

  GD_PIX_gdispLogo = 0,
  GD_PIX_okButton,
  GD_PIX_okButton2, 
  GD_PIX_stopButton,
  GD_PIX_timeDigits,
  GD_PIX_2dPlot,
  GD_PIX_textPlot,
  GD_PIX_stubProvider,
  GD_PIX_resProvider,
  GD_PIX_collapseNode,
  GD_PIX_expandedNode,
  GD_PIX_applyButton,
  GD_PIX_doneButton,
  GD_PIX_warning,
  GD_PIX_error,
  GD_PIX_info,
  GD_PIX_magentaBall,
  GD_PIX_cyanBall,
  GD_PIX_yellowBall,
  GD_PIX_blueBall,
  GD_PIX_greenBall,
  GD_PIX_redBall,
  GD_PIX_NbPixmaps /* Pixmap total number */

} Pixmap_ID;

/*
 * Pixmap type definition.
 */
typedef struct Pixmap_T_ {

  Pixmap_ID   id;
  gchar     **data;
  GdkPixmap  *pixmap;
  GdkBitmap  *mask;
  guint       width;
  guint       height;

} Pixmap_T;

#endif /* __PIXMAPS_H__ */
