/*!  \file 

$Id: gdisp_colormap.c,v 1.3 2004-05-11 19:47:35 esteban Exp $

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

File      : Colormap management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

#define _GD_MAX_COLOR_IN_DEFAULT_COLORMAP_ 170


/*
 * HLS Tool.
 */
static gdouble
gdisp_hlsUtil (gdouble n1,
	       gdouble n2,
	       gdouble hue)
{

  gdouble val = 0.0;
   
  if (hue > 360.0)
    hue -= 360.0;

  if (hue < 000.0)
    hue += 360.0;

  if (hue < 060.0)
    val = n1 + (n2 - n1) * hue / 060.0;
  else if (hue < 180.0)
    val = n2;
  else if (hue < 240.0)
    val = n1 + (n2 - n1) * (240.0 - hue) / 060.0;
  else
    val = n1;

  return val;

}


/*
 * HLS to RGB.
 */
static void 
gdisp_hls2rgb (gdouble  hue,
	       gdouble  light,
	       gdouble  saturation,
	       gdouble *red,
	       gdouble *green,
	       gdouble *blue)

{

  gdouble m1 = 0.0,
          m2 = 0.0;

  if (light < 0.5)
    m2 = light * (1.0 + saturation);
  else
    m2 = light + saturation - light * saturation;

  m1 = 2.0 * light - m2;

  if (saturation == 0.0) {

    *red   = light;
    *green = light;
    *blue  = light;

  }
  else {

    *red   = gdisp_hlsUtil(m1,m2,(gdouble)(hue + 120.0));
    *green = gdisp_hlsUtil(m1,m2,(gdouble)(hue        ));
    *blue  = gdisp_hlsUtil(m1,m2,(gdouble)(hue - 120.0));

  }

}


/*
 * Compute the definition of each color.
 * Set up the right value for RED, GREEN and BLUE components.
 */
static void
gdisp_getColorDefinition (GdkColor *colors)
{

#define _GD_COLOR_COEF_ 255.0

  guint   i     = 0;
  gdouble h     = 0.0,
          l     = 0.0,
          s     = 0.0,
          r     = 0.0,
          g     = 0.0,
          b     = 0.0,
          val   = 0.0,
          incr  = 0.0,
          h_min = 0.0,
          h_max = 0.0;


  /*
   * Create Red color variations.
   */
  r = 0.4;
  g = 0.0;
  b = 0.0;

  for (i=0; i<3; i++) {

    colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
    colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
    colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

    r = r + 0.2;

  }
  
  r = 1.0;
  g = 0.3;
  b = 0.3;

  for (i=3; i<7; i++) {

    colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
    colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
    colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

    g = g + 0.1;
    b = b + 0.1;

  }


  /*
   * Create green color variations.
   */
  r = 0.0;
  g = 0.4;
  b = 0.0;

   for (i=7; i<12; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     g = g + 0.135;

   }

   r = 0.4;
   g = 1.0;
   b = 0.4;

   for (i=12; i<14; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     b = b + 0.2;
     r = r + 0.2;

   }


   /*
    * Create yellow color variations.
    */
   r = 1.0;
   g = 0.6;
   b = 0.0;

   for (i=14; i<19; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     g = g + 0.1;

   }

   r = 1.0;
   g = 1.0;
   b = 0.5;

   for (i=19; i<21; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     b = b + 0.25;

   }


   /*
    * Create blue color variations.
    */
   r = 0.0;
   g = 0.0;
   b = 0.4;

   for (i=21; i<25; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     b = b + 0.2;

   }

   r = 0.30;
   g = 0.42;
   b = 1.00;

   for (i=25; i<28; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     r = r + 0.18;
     g = g + 0.22;

   }


   /*
    * Create magenta color variations.
    */
   r = 0.4;
   g = 0.0;

   for (i=28; i<35; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(r * _GD_COLOR_COEF_);

     r = r + 0.1;
     g = g + 0.7 / 6.0;

   }


   /*
    * Create cyan color variations.
    */
   r = 0.0;
   g = 0.5;
   b = 0.5;

   for (i=35; i<39; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     g = g + 0.15;
     b = b + 0.15;

   }

   r = 0.2;
   g = 1.0;
   b = 1.0;

   for (i=39; i<42; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     r = r + 0.3;

   }


   /*
    * Create variations from black color to white color.
    */
   r = 0.0;

   for (i=42; i<49; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(r * _GD_COLOR_COEF_);

     r = r + 0.5 / 7.0;

   }

   r = 0.5;

   for (i=49; i<56; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(r * _GD_COLOR_COEF_);

     r = r + 0.5 / 8.0;

   }


   /*
    * Create variations from blue to red, passing by green and yellow.
    */
   r = 0.0;
   g = 0.0;
   b = 1.0;

   for (i=56; i<61; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     g = g + 0.2;

   }

   r = 0.0;
   g = 1.0;
   b = 1.0;

   for (i=61;i<63;i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     b = b - 0.5;

   }

   r = 0.0;
   g = 1.0;
   b = 0.0;

   for (i=63; i<65; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);
  
     r = r + 0.5;

   }

   r = 1.0;
   g = 1.0;
   b = 0.0;

   for (i=65; i<70; i++) {

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     g = g - 0.25;

   }


   /*
    * 50 variations from red to blue for post-processing.
    */
   h_min = -10.0;
   h_max = 250.0;

   incr = (h_max - h_min) / 50.0;

   h = h_min;
   l = 0.5;
   s = 1.0;

   for (i=70; i<120; i++) {

     gdisp_hls2rgb(h,l,s,&r,&g,&b);

     colors[i].red   = (gushort)(r * _GD_COLOR_COEF_);
     colors[i].green = (gushort)(g * _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(b * _GD_COLOR_COEF_);

     h += incr;

   }


   /*
    * 50 variations from white to black for post-processing.
    */
   val = 15000.0;
   for (i=120; i<170; i++) {

     val = val + 1000.0;

     colors[i].red   = (gushort)(val / _GD_COLOR_COEF_);
     colors[i].green = (gushort)(val / _GD_COLOR_COEF_);
     colors[i].blue  = (gushort)(val / _GD_COLOR_COEF_);

   }


   /*
    * Change red, green, blue intervals.
    */
   for (i=0; i<_GD_MAX_COLOR_IN_DEFAULT_COLORMAP_; i++) {

     colors[i].red   *= 257;
     colors[i].green *= 257;
     colors[i].blue  *= 257;

   }

}


/*
 * Returns position of highest set bit in 'ul'
 * as an integer (0-31), or -1 if none.
 */
#if defined(GDISP_FIRST_METHOD)

static gint
gdisp_highBit(gulong ul)
{

   gint    i = 0;
   gulong hb = 0x8000;

   hb = hb << 16;
   for (i=31; ((ul & hb) == 0) && i>=0; i--, ul<<=1);

   return i;

}

#endif


/*
 * Deduce pixel information from RGB inputs and visual parameters.
 * TRUE_COLOR visual specific.
 */
static gulong
gdisp_rgb2pixel (GdkVisual *visual,
		 gushort    red,
		 gushort    green,
		 gushort    blue)
{

#if !defined(GDISP_FIRST_METHOD)

  return (gulong)
         (((red   >> (16 - visual->red_prec  )) << visual->red_shift  ) |
          ((green >> (16 - visual->green_prec)) << visual->green_shift) |
          ((blue  >> (16 - visual->blue_prec )) << visual->blue_shift ));

#else

  gulong ulRed      = (gulong)red,
         ulGreen    = (gulong)green,
         ulBlue     = (gulong)blue;
  gint   redShift   = 0,
         greenShift = 0,
         blueShift  = 0;
    

  /*
   * Shift r,g,b so that high bit of 16-bit color specification is 
   * aligned with high bit of r,g,b-mask in visual, 
   * AND each component with its mask, and OR the three components together.
   */
  redShift   = 15 - gdisp_highBit(visual->red_mask  );
  greenShift = 15 - gdisp_highBit(visual->green_mask);
  blueShift  = 15 - gdisp_highBit(visual->blue_mask );

  if (redShift < 0)
    ulRed = ulRed     << (-redShift);
  else
    ulRed = ulRed     >>   redShift;

  if (greenShift < 0)
    ulGreen = ulGreen << (-greenShift);
  else
    ulGreen = ulGreen >>   greenShift;

  if (blueShift < 0)
    ulBlue = ulBlue   << (-blueShift);
  else
    ulBlue = ulBlue   >>   blueShift;

  ulRed   = ulRed   & visual->red_mask;
  ulGreen = ulGreen & visual->green_mask;
  ulBlue  = ulBlue  & visual->blue_mask;

  return (red | green | blue);

#endif

}



/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Affect a color to a provider.
 */
GdkColor*
gdisp_getProviderColor (Kernel_T *kernel,
			gint      providerId)
{

  /*
   * Definition of colors affected to each provider.
   */
  gint providerColors[]  = { 6, 10, 16, 27, 33, 37 };
  gint providerColorSize = GD_MAX_PROVIDER_NUMBER;

  assert(kernel->colors);

  if (providerId < providerColorSize) {

    return &kernel->colors[providerColors[providerId]];

  }
  else {

    return (GdkColor*)NULL;

  }

}


/*
 * Create GDISP+ colormap.
 */
void
gdisp_createColormap (Kernel_T *kernel)
{

  gboolean *successTable    = (gboolean*)NULL;
  gint      allocatedColors = 0,
            colorCpt        = 0;
  GString  *messageString   = (GString*)NULL;


  assert(kernel);

  /*
   * Get back system colormap.
   * FIXME : Work with an enhanced colormap system management.
   */
  kernel->colormap = gdk_colormap_get_system();
  kernel->visual   = gdk_colormap_get_visual(kernel->colormap);


  /*
   * Define all requested colors.
   */
  kernel->colorNumber = _GD_MAX_COLOR_IN_DEFAULT_COLORMAP_;
  kernel->colors      = (GdkColor*)g_malloc0(kernel->colorNumber *
					     sizeof(GdkColor));
  assert(kernel->colors);


  /*
   * Get back color definition.
   */
  gdisp_getColorDefinition(kernel->colors);


  /*
   * Try to allocate these colors.
   * Watch out visual type !!!
   * Colors are not writable, thus they can be shared among applications.
   */
  switch (kernel->visual->type) {

  case GDK_VISUAL_PSEUDO_COLOR :

    successTable = (gboolean*)g_malloc0(kernel->colorNumber *
					sizeof(gboolean));
    assert(successTable);

    allocatedColors = gdk_colormap_alloc_colors(kernel->colormap,
						kernel->colors,
						kernel->colorNumber,
						FALSE, /* not writable     */
						TRUE,  /* perform matching */
						successTable);

    g_free(successTable);
    successTable = (gboolean*)NULL;
    break;

  case GDK_VISUAL_TRUE_COLOR :

    for (colorCpt=0; colorCpt<kernel->colorNumber; colorCpt++) {

      kernel->colors[colorCpt].pixel =
	gdisp_rgb2pixel(kernel->visual,
			kernel->colors[colorCpt].red,
			kernel->colors[colorCpt].green,
			kernel->colors[colorCpt].blue);

#if defined(GD_COLORMAP_DEBUG)
      fprintf(stdout,
	      "Color %d : r=%d, g=%d, b=%d, pixel = %ld\n",
	      colorCpt + 1,
	      kernel->colors[colorCpt].red,
	      kernel->colors[colorCpt].green,
	      kernel->colors[colorCpt].blue,
	      kernel->colors[colorCpt].pixel);
#endif

    }

#if defined(GD_COLORMAP_DEBUG)
    fflush(stdout);
#endif

    /* No problem for allocating all colors in TRUECOLOR */
    allocatedColors = kernel->colorNumber;
    break;

  default :
    break;

  }


  /*
   * Output message.
   */
  messageString = g_string_new((gchar*)NULL);
  if (allocatedColors == kernel->colorNumber) {

    g_string_sprintf(messageString,
		     "All %d requested colors have been allocated.",
		     kernel->colorNumber);
    kernel->outputFunc(kernel,messageString,GD_MESSAGE);
  

  }
  else {

    g_string_sprintf(messageString,
		     "Only %d colors out of %d have been allocated.",
		     allocatedColors,
		     kernel->colorNumber);
    kernel->outputFunc(kernel,messageString,GD_WARNING);

  }

}


/*
 * Destroy GDISP+ colormap.
 */
void
gdisp_destroyColormap (Kernel_T *kernel)
{


  /*
   * Free all colors into the colormap.
   */
  if (kernel->visual->type == GDK_VISUAL_PSEUDO_COLOR) {

    gdk_colormap_free_colors(kernel->colormap,
			     kernel->colors,
			     kernel->colorNumber);

  }


  /*
   * Free mempry.
   */
  g_free(kernel->colors);

  kernel->colors      = (GdkColor*)NULL;
  kernel->colorNumber = 0;

}

