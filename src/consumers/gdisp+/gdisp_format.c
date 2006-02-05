/*!  \file 

$Id: gdisp_format.c,v 1.2 2006-02-05 18:02:36 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2004 - Euskadi

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

File      : Format management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gdisp_format.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Conversion to ASCII format.
 */
static void
gdisp_formatInteger64toAscii ( guint64   uInteger64,
			       Format_T  format,
			       gchar    *outputBuffer )
{

  gint     shift    = 7 * sizeof(guint64);
  guchar   car      = 0;

  /*
   * Loop over all bits.
   */
  for (; shift>=0; shift-=8) {

    car = (uInteger64 >> shift) & 0xFF;
    *outputBuffer++ = toascii((gint)car) ? car : '_';
    *outputBuffer++ = ' ';

  }

  /*
   * Terminate the string.
   */
  *outputBuffer = '\0';

}


/*
 * Conversion to binary format.
 */
static void
gdisp_formatInteger64toBinary ( guint64   uInteger64,
				Format_T  format,
				gchar    *outputBuffer )
{

  gint     shift    = (8 * sizeof(guint64)) - 1;
  guint    bit      = 0;
  gboolean writeBit = FALSE;

  /*
   * Loop over all bits.
   */
  if (uInteger64 == 0) {

    *outputBuffer++ = '0';

  }
  else for (; shift>=0; shift--) {

    bit = (uInteger64 >> shift) & 0x1;

    /* avoid writing leading zeros */
    if (writeBit == FALSE && bit == 1) {
      writeBit = TRUE;
    }

    if (writeBit == TRUE) {
      *outputBuffer++ = bit ? '1' : '0';
    }
  }

  /*
   * Terminate the string.
   */
  *outputBuffer++ = ' ';
  *outputBuffer++ = 'b';
  *outputBuffer   = '\0';

}


/*
 * Conversion to hexadecimal format.
 */
static void
gdisp_formatInteger64toHexadecimal ( guint64   uInteger64,
				     Format_T  format,
				     gchar    *outputBuffer )
{

  /*
   * Format unsigned integer 64 according to requested format.
   */
  if (format == GD_HEXADECIMAL_1) {

    sprintf(outputBuffer,
	    "0x %08X%08X ",
	    (guint32)((uInteger64 >> 32) & 0xFFFFFFFF),
	    (guint32)((uInteger64      ) & 0xFFFFFFFF));

  }
  else if (format == GD_HEXADECIMAL_2) {

    sprintf(outputBuffer,
	    "0x %08X %08X ",
	    (guint32)((uInteger64 >> 32) & 0xFFFFFFFF),
	    (guint32)((uInteger64      ) & 0xFFFFFFFF));

  }
  else if (format == GD_HEXADECIMAL_4) {

    sprintf(outputBuffer,
	    "0x %04X %04X %04X %04X",
	    (guint32)((uInteger64 >> 48) & 0xFFFF),
	    (guint32)((uInteger64 >> 32) & 0xFFFF),
	    (guint32)((uInteger64 >> 16) & 0xFFFF),
	    (guint32)((uInteger64      ) & 0xFFFF));

  }
  else if (format == GD_HEXADECIMAL_8) {

    sprintf(outputBuffer,
	    "0x %02X %02X %02X %02X %02X %02X %02X %02X ",
	    (guint32)((uInteger64 >> 56) & 0xFF),
	    (guint32)((uInteger64 >> 48) & 0xFF),
	    (guint32)((uInteger64 >> 40) & 0xFF),
	    (guint32)((uInteger64 >> 32) & 0xFF),
	    (guint32)((uInteger64 >> 24) & 0xFF),
	    (guint32)((uInteger64 >> 16) & 0xFF),
	    (guint32)((uInteger64 >>  8) & 0xFF),
	    (guint32)((uInteger64      ) & 0xFF));

  }


}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Get back the label of a given format.
 * Long labels to be put onto the GTK GUI.
 */
gchar*
gdisp_getFormatLabel ( Format_T format )
{

  char *labelTable[GD_MAX_FORMATS] = { "Default",
				       "Hex. 1 block",
				       "Hex. 2 blocks",
				       "Hex. 4 blocks",
				       "Hex. 8 blocks",
				       "Binary",
				       "0.0",
				       "0.00",
				       "0.000",
				       "0.0000",
				       "0.00000",
				       "0.000000",
				       "0.0000000",
				       "0.00000000",
				       "0.000000000",
				       "0.0000000000",
				       "Scientific",
				       "Printable Ascii Code" };

  return (format < GD_MAX_FORMATS ? labelTable[format] : "<<<error>>>");

}


/*
 * Get back the label of a given format.
 * Small label to be put into the configuration.
 */
gchar*
gdisp_getFormatSmallLabel ( Format_T *format,
			    gchar    *formatAsString )
{

  char *labelTable[GD_MAX_FORMATS] = { "Default",
				       "Hex.1",
				       "Hex.2",
				       "Hex.4",
				       "Hex.8",
				       "Binary",
				       "Ffd.1",
				       "Ffd.2",
				       "Ffd.3",
				       "Ffd.4",
				       "Ffd.5",
				       "Ffd.6",
				       "Ffd.7",
				       "Ffd.8",
				       "Ffd.9",
				       "Ffd.10",
				       "Scientific",
				       "Ascii" };

  if (formatAsString == (gchar*)NULL) {

    /*
     * Conversion Format_T --> format as a string.
     */
    return (*format < GD_MAX_FORMATS ? labelTable[*format] : "<<<error>>>");

  }
  else {

    /*
     * conversion format as a string --> Format_T.
     */
    for ((*format) = GD_DEFAULT_FORMAT;
	 (*format) < GD_MAX_FORMATS;
	 (*format)++) {

      if (strcmp(labelTable[*format],formatAsString) == 0) {
	return (gchar*)NULL;
      }

    } /* for */

    /*
     * Fallback.
     */
    *format = GD_DEFAULT_FORMAT;

  } /* else */

  return (gchar*)NULL;

}


/*
 * Format a double value into a string buffer.
 */
void
gdisp_formatDoubleValue ( gdouble   inputValue,
			  Format_T  format,
			  gchar    *outputBuffer )
{

  gchar ffFormat[12];

  /*
   * Format input double precision real according to requested format.
   */
  switch (format) {

  case GD_DEFAULT_FORMAT :
    sprintf(outputBuffer,"%g ",inputValue);
    break;

  case GD_FLOATING_FIXED_1 :
  case GD_FLOATING_FIXED_2 :
  case GD_FLOATING_FIXED_3 :
  case GD_FLOATING_FIXED_4 :
  case GD_FLOATING_FIXED_5 :
  case GD_FLOATING_FIXED_6 :
  case GD_FLOATING_FIXED_7 :
  case GD_FLOATING_FIXED_8 :
  case GD_FLOATING_FIXED_9 :
  case GD_FLOATING_FIXED_10 :
    sprintf(ffFormat,"%c.%df ",'%',format - GD_FLOATING_FIXED_1 + 1);
    sprintf(outputBuffer,ffFormat,inputValue);
    break;

  case GD_HEXADECIMAL_1 :
  case GD_HEXADECIMAL_2 :
  case GD_HEXADECIMAL_4 :
  case GD_HEXADECIMAL_8 :
    gdisp_formatInteger64toHexadecimal((guint64)inputValue,
				       format,
				       outputBuffer);
    break;

  case GD_SCIENTIFIC :
    sprintf(outputBuffer,"%e ",inputValue);
    break;

  case GD_BINARY :
    gdisp_formatInteger64toBinary((guint64)inputValue,
				  format,
				  outputBuffer);
    break;

  case GD_ASCII :
    gdisp_formatInteger64toAscii((guint64)inputValue,
				 format,
				 outputBuffer);
    break;

  case GD_MAX_FORMATS :
  default :
    sprintf(outputBuffer,"Not implemented");
    break;

  }

}



