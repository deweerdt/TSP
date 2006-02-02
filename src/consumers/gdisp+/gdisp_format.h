/*!  \file 

$Id: gdisp_format.h,v 1.1 2006-02-02 21:03:32 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi

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

File      : Definition of all formats.

-----------------------------------------------------------------------
*/

#ifndef __FORMAT_H__
#define __FORMAT_H__

#include <glib.h>

/*
 * Available Formats.
 *
 * 1) hexadecimal (with packets of 1 / 2 / 4 / 8 bytes), for example
 *           FF AA BB 34 11 22 22 66
 *     or    FFAA BB34 1122 2266
 *     or    FFAABB34 11222266
 *     or    FFAABB3411222266
 *
 * 2) binary
 *           10111001 10011011
 *
 * 3) floating fixed decimal
 *           3.1234 (with fixed decimal number)
 *
 * 4) scientific
 *           3.1234E-5
 *
 * 5) ASCII
 *    convert each byte to ASCII mode
 *    see : #include <ctype.h>
 *          int toascii (int c);
 *          int isascii (int c);
 *          ...etc... 
 *
 */
typedef enum {

  GD_DEFAULT_FORMAT,
  GD_HEXADECIMAL_1,
  GD_HEXADECIMAL_2,
  GD_HEXADECIMAL_4,
  GD_HEXADECIMAL_8,
  GD_BINARY,
  GD_FLOATING_FIXED,
  GD_SCIENTIFIC,
  GD_ASCII,
  GD_MAX_FORMATS

} Format_T;


/*
 * Prototypes.
 */
gchar *gdisp_getFormatLabel    ( Format_T  format );

void   gdisp_formatDoubleValue ( gdouble   inputValue,
				 Format_T  format,
				 gchar    *outputBuffer );

#endif /* __FORMAT_H__ */
