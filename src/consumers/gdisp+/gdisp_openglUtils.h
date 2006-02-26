/*

$Id: gdisp_openglUtils.h,v 1.2 2006-02-26 14:08:23 erk Exp $

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

File      : GDISP OpenGL utilities header file.
            This is the toolkit that enables an OpenGL rendering on a
            GTK drawing area.

-----------------------------------------------------------------------
*/

#ifndef __OPENGL_UTILS_H__
#define __OPENGL_UTILS_H__

/*
 * GTK & GDK.
 */
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

/*
 * OpenGL & GLX Extension.
 */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

/*
 * GDISP+
 */
#include "gdisp_kernel.h"

/*
 --------------------------------------------------------------------
                             TYPE DEFINITION
 --------------------------------------------------------------------
*/

/*
 * Window Types
 */
#define OPENGL_RGB        0
#define OPENGL_INDEX      1
#define OPENGL_SINGLE     0
#define OPENGL_DOUBLE     2
#define OPENGL_DIRECT     0
#define OPENGL_INDIRECT   4
#define OPENGL_ACCUM      8
#define OPENGL_ALPHA     16
#define OPENGL_DEPTH     32
#define OPENGL_OVERLAY   64
#define OPENGL_UNDERLAY 128
#define OPENGL_STENCIL  512

/* 
 * Window Masks
 */
#define OPENGL_IS_RGB(x)       (((x) & OPENGL_INDEX)    == 0)
#define OPENGL_IS_INDEX(x)     (((x) & OPENGL_INDEX)    != 0)
#define OPENGL_IS_SINGLE(x)    (((x) & OPENGL_DOUBLE)   == 0)
#define OPENGL_IS_DOUBLE(x)    (((x) & OPENGL_DOUBLE)   != 0)
#define OPENGL_IS_DIRECT(x)    (((x) & OPENGL_INDIRECT) == 0)
#define OPENGL_IS_INDIRECT(x)  (((x) & OPENGL_INDIRECT) != 0)
#define OPENGL_HAS_ACCUM(x)    (((x) & OPENGL_ACCUM)    != 0)
#define OPENGL_HAS_ALPHA(x)    (((x) & OPENGL_ALPHA)    != 0)
#define OPENGL_HAS_DEPTH(x)    (((x) & OPENGL_DEPTH)    != 0)
#define OPENGL_HAS_OVERLAY(x)  (((x) & OPENGL_OVERLAY)  != 0)
#define OPENGL_HAS_UNDERLAY(x) (((x) & OPENGL_UNDERLAY) != 0)
#define OPENGL_HAS_STENCIL(x)  (((x) & OPENGL_STENCIL)  != 0)

/*
 * Structure.
 */
typedef struct OpenGL_T_ {

  /* GTK resources for OpenGL drawings */

  GdkWindow   *openglParentWindow;
  GdkWindow   *openglWindow;

  GdkVisual   *openglVisual;
  GdkColormap *openglColormap;

  guint        openglWindowWidth;
  guint        openglWindowHeight;

  /* Low level GLX and X resources */

  Display     *xDisplay;
  int          xScreen;
  XVisualInfo *xVisualInfo;

  GLXContext   glXContext;
  GLenum       glXType;

} OpenGL_T;


/*
 --------------------------------------------------------------------
                             PUBLIC API
 --------------------------------------------------------------------
*/

OpenGL_T*
gdisp_connectToOpenGL ( Kernel_T  *kernel,
			GtkWidget *hostWidget,
			guint      hostWidth,
			guint      hostHeight,
			GLenum     openglRessources );

void
gdisp_disconnectFromOpenGL ( OpenGL_T *opengl );

void
gdisp_grabOpenGL ( OpenGL_T *opengl );

#endif /* __OPENGL_UTILS_H__ */
