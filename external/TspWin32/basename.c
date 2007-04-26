/*

$Id: basename.c,v 1.2 2007-04-26 17:51:30 deweerdt Exp $

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : TspWIn32

-----------------------------------------------------------------------

Purpose   : Main implementation of the emulation of the basename UNIX function under Windows 

-----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include "basename.h"

char *basename(char *path)
{   
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   static char fname[_MAX_FNAME];
   char ext[_MAX_EXT];

   _splitpath( path, drive, dir, fname, ext );
   
   strcat(fname, ext);

   return fname;
}