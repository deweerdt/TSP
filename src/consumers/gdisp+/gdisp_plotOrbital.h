/*

$Id: gdisp_plotOrbital.h,v 1.3 2006-02-26 14:08:24 erk Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi, Yves DUFRENNE

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

File      : Definition of 'orbital plot' private structures.

-----------------------------------------------------------------------
*/

#ifndef __ORBITAL_PLOT_H__
#define __ORBITAL_PLOT_H__

/*
 * For OpenGL mapping needs.
 */
#include "gdisp_openglUtils.h"


/*
 * Private structure of an 'orbital plot'.
 */
typedef struct PlotOrbital_T_ {

  /*
   * Attributes.
   */
  PlotType_T  poType;

  /*
   * List of pointer on TSP_Symbol_T.
   */
  GList      *poSymbolList;

  /*
   * Graphic widget.
   */
  GtkWidget  *poGraphicArea;
  guint       poWidth;
  guint       poHeight;

  /*
   * Parent widget.
   */
  GtkWidget  *poParent;

  /*
   * OpenGL mapping.
   */
  OpenGL_T   *poOpengl;

} PlotOrbital_T;


#endif /* __ORBITAL_PLOT_H__ */
