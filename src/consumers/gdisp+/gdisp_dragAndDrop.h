/*!  \file 

$Id: gdisp_dragAndDrop.h,v 1.1 2004-02-04 20:32:09 esteban Exp $

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

File      : Drag And Drop Atom definitions.

-----------------------------------------------------------------------
*/

#ifndef __DND_H__
#define __DND_H__


/*
 * Drag And Drop Atom definitions.
 */


/*
 * DND data format type idenfication :
 * It identifies what format the drag data is for internal parsing
 * by this program. Remember that this is the data format type (not
 * to be confused with data type, like string, binary, etc).
 *
 * These values are passed to 'gtk_drag_dest_set()' and
 * 'gtk_drag_source_set()' and will be given as inputs in DND signal
 * callbacks.
 *
 * In each callback, we have the choice of using either the name 
 * (a string) or the info (an int). In GDISP+, we will use the info
 * (an int) since it is easier and more commonly practiced.
 *
 */
#define GD_DND_TARGET_NAME "GDISP_STRING"
#define GD_DND_TARGET_INFO 0

#define GD_DND_SYMBOL_LIST_EXCHANGE "symbolListExchange"

#endif /* __DND_H__ */
