/*!  \file 

$Id: gdisp_openglUtils.c,v 1.1 2005-03-10 21:38:36 esteban Exp $

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

File      : GDISP OpenGL utilities.
            This is the toolkit that enables an OpenGL rendering on a
            GTK drawing area.

-----------------------------------------------------------------------
*/

/*
 * System includes.
 */
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "gdisp_openglUtils.h"

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

static GLenum
gdisp_getWindowType ( OpenGL_T *opengl )
{

  GLenum mask = 0;
  gint   x    = 0;
  gint   y    = 0;
  gint   z    = 0;

  glXGetConfig(opengl->xDisplay,
	       opengl->xVisualInfo,
	       GLX_DOUBLEBUFFER,
	       &x);

  if (x) {

    mask |= OPENGL_DOUBLE;

  } else {

    mask |= OPENGL_SINGLE;

  }

  glXGetConfig(opengl->xDisplay,
	       opengl->xVisualInfo,
	       GLX_RGBA,
	       &x);

  if (x) {

    mask |= OPENGL_RGB;

    glXGetConfig(opengl->xDisplay,
		 opengl->xVisualInfo,
		 GLX_ALPHA_SIZE,
		 &x);

    if (x > 0) {

      mask |= OPENGL_ALPHA;

    }

    glXGetConfig(opengl->xDisplay,
		 opengl->xVisualInfo,
		 GLX_ACCUM_RED_SIZE,
		 &x);

    glXGetConfig(opengl->xDisplay,
		 opengl->xVisualInfo,
		 GLX_ACCUM_GREEN_SIZE,
		 &y);

    glXGetConfig(opengl->xDisplay,
		 opengl->xVisualInfo,
		 GLX_ACCUM_BLUE_SIZE,
		 &z);

    if (x > 0 && y > 0 && z > 0) {

      mask |= OPENGL_ACCUM;

    }

  } else {

    mask |= OPENGL_INDEX;

  }

  glXGetConfig(opengl->xDisplay,
	       opengl->xVisualInfo,
	       GLX_DEPTH_SIZE,
	       &x);

  if (x > 0) {

    mask |= OPENGL_DEPTH;

  }

  glXGetConfig(opengl->xDisplay,
	       opengl->xVisualInfo,
	       GLX_STENCIL_SIZE,
	       &x);

  if (x > 0) {

    mask |= OPENGL_STENCIL;

  }

  if (glXIsDirect(opengl->xDisplay,
		  opengl->glXContext)) {

    mask |= OPENGL_DIRECT;

  } else {

    mask |= OPENGL_INDIRECT;

  }

  return mask;

}

static XVisualInfo*
gdisp_findBestVisual ( OpenGL_T *opengl,
		       GLenum    type )
{

  gint  list[32];
  guint i = 0;

  list[i++] = GLX_LEVEL;
  list[i++] = 0;

  if (OPENGL_IS_DOUBLE(type)) {

    list[i++] = GLX_DOUBLEBUFFER;

  }

  if (OPENGL_IS_RGB(type)) {

    list[i++] = GLX_RGBA;
    list[i++] = GLX_RED_SIZE;
    list[i++] = 1;
    list[i++] = GLX_GREEN_SIZE;
    list[i++] = 1;
    list[i++] = GLX_BLUE_SIZE;
    list[i++] = 1;

    if (OPENGL_HAS_ALPHA(type)) {

      list[i++] = GLX_ALPHA_SIZE;
      list[i++] = 1;

    }

    if (OPENGL_HAS_ACCUM(type)) {

      list[i++] = GLX_ACCUM_RED_SIZE;
      list[i++] = 1;
      list[i++] = GLX_ACCUM_GREEN_SIZE;
      list[i++] = 1;
      list[i++] = GLX_ACCUM_BLUE_SIZE;
      list[i++] = 1;

      if (OPENGL_HAS_ALPHA(type)) {

	list[i++] = GLX_ACCUM_ALPHA_SIZE;

	list[i++] = 1;

      }

    }

  }
  else if (OPENGL_IS_INDEX(type)) {

    list[i++] = GLX_BUFFER_SIZE;
    list[i++] = 1;

  }

  if (OPENGL_HAS_DEPTH(type)) {

    list[i++] = GLX_DEPTH_SIZE;
    list[i++] = 1;

  }

  if (OPENGL_HAS_STENCIL(type)) {

    list[i++] = GLX_STENCIL_SIZE;
    list[i++] = 1;

  }

  list[i] = (int)None;

  return glXChooseVisual(opengl->xDisplay,
			 opengl->xScreen,
			 list);

}

static void
gdisp_setRGBMap ( OpenGL_T *opengl,
		  GdkColor *colors,
		  gint      colorNumber )
{

  GdkColor gdkColor;
  gint     rShift = 0;
  gint     gShift = 0;
  gint     bShift = 0;
  guint    max    = 0;
  guint    i      = 0;

  max = (colorNumber > opengl->xVisualInfo->colormap_size) ?
    opengl->xVisualInfo->colormap_size : colorNumber;

  switch (opengl->xVisualInfo->class) {

  case DirectColor:

    rShift = ffs((unsigned int)opengl->xVisualInfo->red_mask  ) - 1;
    gShift = ffs((unsigned int)opengl->xVisualInfo->green_mask) - 1;
    bShift = ffs((unsigned int)opengl->xVisualInfo->blue_mask ) - 1;

    for (i = 0; i < max; i++) {

      gdkColor.pixel = ((i << rShift) & opengl->xVisualInfo->red_mask  ) |
	               ((i << gShift) & opengl->xVisualInfo->green_mask) |
	               ((i << bShift) & opengl->xVisualInfo->blue_mask );

      gdkColor.red   = (unsigned short)colors[i].red;
      gdkColor.green = (unsigned short)colors[i].green;
      gdkColor.blue  = (unsigned short)colors[i].blue;

      gdk_colors_store(opengl->openglColormap,
		       &gdkColor,
		       1);
      
    }
    break;

  case GrayScale:
  case PseudoColor:

    for (i = 0; i < max; i++) {

      gdkColor.pixel = i;

      gdkColor.red   = (unsigned short)colors[i].red;
      gdkColor.green = (unsigned short)colors[i].green;
      gdkColor.blue  = (unsigned short)colors[i].blue;

      gdk_colors_store(opengl->openglColormap,
		       &gdkColor,
		       1);
    }
    break;

  }

}

static GdkColormap*
gdisp_createColormap ( OpenGL_T *opengl )
{

  GdkColormap *gdkColormap = (GdkColormap*)NULL;

  if (OPENGL_IS_INDEX(opengl->glXType)) {

    if (opengl->xVisualInfo->class != StaticColor &&
	opengl->xVisualInfo->class != StaticGray     ) {

      gdkColormap = gdk_colormap_new(opengl->openglVisual,
				     TRUE);

    }
    else {

      gdkColormap = gdk_colormap_new(opengl->openglVisual,
				     FALSE);

    }

  }
  else {

    gdkColormap = gdk_colormap_new(opengl->openglVisual,
				   FALSE);

  }

  return gdkColormap;

}

static GdkWindow*
gdisp_createOpenGLWindow ( OpenGL_T *opengl )
{

  GdkWindow     *gdkWindow = (GdkWindow*)NULL;
  GdkWindowAttr  gdkWindowAttributes;

  memset(&gdkWindowAttributes,0,sizeof(GdkWindowAttr));

  /*
   * Set up window attributes.
   */
  gdkWindowAttributes.x             = 0;
  gdkWindowAttributes.y             = 0;
  gdkWindowAttributes.width         = opengl->openglWindowWidth;
  gdkWindowAttributes.height        = opengl->openglWindowHeight;
  gdkWindowAttributes.visual        = opengl->openglVisual;
  gdkWindowAttributes.colormap      = opengl->openglColormap;
  gdkWindowAttributes.window_type   = GDK_WINDOW_CHILD;
  gdkWindowAttributes.wclass        = GDK_INPUT_OUTPUT;
  gdkWindowAttributes.event_mask    = GDK_EXPOSURE_MASK |
                                      GDK_STRUCTURE_MASK;

  /*
   * Create the GDK window.
   */
  gdkWindow = gdk_window_new(opengl->openglParentWindow,
			     &gdkWindowAttributes,
			     GDK_WA_X        |
			     GDK_WA_Y        |
			     GDK_WA_COLORMAP |
			     GDK_WA_VISUAL);

  gdk_window_show(gdkWindow);

  return gdkWindow;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Stop the mapping of an openGL rendering onto a GTK widget.
 */
void
gdisp_disconnectFromOpenGL ( OpenGL_T *opengl )
{

  if (opengl == (OpenGL_T*)NULL) {
    return;
  }

  if (opengl->openglWindow != (GdkWindow*)NULL) {
    gdk_window_destroy(opengl->openglWindow);
  }

  if (opengl->openglColormap != (Colormap)NULL) {
    gdk_colormap_unref(opengl->openglColormap);
  }

  if (opengl->glXContext != (GLXContext)NULL) {
    glXDestroyContext(opengl->xDisplay,
		      opengl->glXContext);
  }

  if (opengl->openglVisual != (GdkVisual*)NULL) {

    /*
     * I do not really know whether I have to free
     * that memory, because I got it from GDK via the
     * gdkx_visual_get function call.
     * I bet it is a copy of the pointer, not of the content.
     */

  }

  if (opengl->xVisualInfo != (XVisualInfo*)NULL) {
    XFree((char*)opengl->xVisualInfo);
  }

  memset(opengl,0,sizeof(OpenGL_T));

  g_free(opengl);

}


/*
 * Start the mapping of an openGL rendering onto a GTK widget.
 */
OpenGL_T*
gdisp_connectToOpenGL ( Kernel_T  *kernel,
			GtkWidget *hostWidget,
			guint      hostWidth,
			guint      hostHeight,
			GLenum     openglRessources )
{

  OpenGL_T *opengl         = (OpenGL_T*)NULL;
  gboolean  glxIsSupported = FALSE;
  gint      glxErrorBase   = 0;
  gint      glxEventBase   = 0;
  Bool      isCurrent      = False; /* X boolean, not GDK one */

  /*
   * Allocate memory for OpenGL session.
   */
  opengl = g_malloc0(sizeof(OpenGL_T));
  if (opengl == (OpenGL_T*)NULL) {

    return (OpenGL_T*)NULL;

  }

  /*
   * Check X connection and GLX extension support.
   */
  opengl->xDisplay = GDK_DISPLAY();

#if defined(GDISP_OPENGL_DEBUG)
  printf("Display is 0x%x\n",opengl->xDisplay);
#endif

  glxIsSupported = glXQueryExtension(opengl->xDisplay,
				     &glxErrorBase,
				     &glxEventBase);

#if defined(GDISP_OPENGL_DEBUG)
  printf("GLX is %ssupported.\n",
	 glxIsSupported == TRUE ? "" : "NOT ");
#endif

  if (glxIsSupported == FALSE) {

    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

  opengl->xScreen = DefaultScreen(opengl->xDisplay);

  /*
   * Find best visual according to OpenGL demand.
   */
  opengl->xVisualInfo = gdisp_findBestVisual(opengl,
					     openglRessources);

#if defined(GDISP_OPENGL_DEBUG)
  printf("X Visual Info is 0x%x\n",opengl->xVisualInfo);
#endif

  if (opengl->xVisualInfo == (XVisualInfo*)NULL) {

    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

  /*
   * Deduce GDK visual from X visual.
   */
  opengl->openglVisual = gdkx_visual_get(opengl->xVisualInfo->visualid);

#if defined(GDISP_OPENGL_DEBUG)
  printf("GDK Visual is 0x%x\n",opengl->openglVisual);
#endif

  if (opengl->openglVisual == (GdkVisual*)NULL) {

    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

  /*
   * Create a GLX context from the X visual.
   */
  opengl->glXContext = glXCreateContext(opengl->xDisplay,
					opengl->xVisualInfo,
					None,      /* no sharing */
					GL_FALSE); /* not direct */

  if (opengl->glXContext == (GLXContext)NULL) {

#if defined(GDISP_OPENGL_DEBUG)
    printf("Cannot create GLX Context.\n");
#endif
    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

#if defined(GDISP_OPENGL_DEBUG)
  printf("GLXContext is 0x%x\n",opengl->glXContext);
#endif

  /*
   * Deduce final attriutes of the GLX context.
   */
  opengl->glXType = gdisp_getWindowType(opengl);

  /*
   * Create a private colormap, because we cannot use default one.
   */
  opengl->openglColormap = gdisp_createColormap(opengl);

  if (opengl->openglColormap == (Colormap)NULL) {

#if defined(GDISP_OPENGL_DEBUG)
    printf("Cannot create specific colormap.\n");
#endif
    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

  /*
   * Fill colormap.
   */
  gdisp_setRGBMap(opengl,
		  kernel->colors,
		  kernel->colorNumber);

  /*
   * Create final window.
   */
  opengl->openglParentWindow = hostWidget->window;

  if (opengl->openglParentWindow == (GdkWindow*)NULL) {

#if defined(GDISP_OPENGL_DEBUG)
    printf("Parent window does not exist yet.\n");
#endif
    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

  opengl->openglWindowWidth  = hostWidth;
  opengl->openglWindowHeight = hostHeight;

  opengl->openglWindow       = gdisp_createOpenGLWindow(opengl);

  if (opengl->openglWindow == (GdkWindow*)NULL) {

#if defined(GDISP_OPENGL_DEBUG)
    printf("Cannot create specific window.\n");
#endif
    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

  /*
   * Flush before coupling together GDK and OpenGL.
   */
  gdk_flush();

  /*
   * That's it... Plug OpenGL into GDK...
   */
  isCurrent = glXMakeCurrent(opengl->xDisplay,
			     GDK_WINDOW_XWINDOW(opengl->openglWindow),
			     opengl->glXContext);

  if (isCurrent == False) {

#if defined(GDISP_OPENGL_DEBUG)
    printf("Can't make window current drawable.\n");
#endif
    gdisp_disconnectFromOpenGL(opengl);
    return (OpenGL_T*)NULL;

  }

  return opengl;

}


void
gdisp_grabOpenGL ( OpenGL_T *opengl )
{

  Bool isCurrent = False; /* X boolean, not GDK one */

  /*
   * That's it... Plug OpenGL into GDK...
   */
  isCurrent = glXMakeCurrent(opengl->xDisplay,
			     GDK_WINDOW_XWINDOW(opengl->openglWindow),
			     opengl->glXContext);

}
