/*

$Header: /home/def/zae/tsp/tsp/external/TspWin32/basename.h,v 1.1 2006-10-18 08:27:56 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Alan PRAT

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
Maintainer : tsp@astrium.eads.net
Component : TspWin32

-----------------------------------------------------------------------

Purpose   : Emulation of the basename UNIX function under Windows 
-----------------------------------------------------------------------
 */

#ifndef __BASENAME_H
#define __BASENAME_H

#include "TspWin32.h"

_EXPORT_TSPWIN32 char *basename(char *path); 

#endif
