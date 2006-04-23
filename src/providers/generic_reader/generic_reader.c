/*

$Id: generic_reader.c,v 1.2 2006-04-23 15:37:48 erk Exp $

-----------------------------------------------------------------------
 
TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2006 Eric NOULARD and Arnaud MORVAN 

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the generic reader

-----------------------------------------------------------------------
*/
#include <string.h>
#include <stdio.h>

#include <tsp_sys_headers.h>
#include <tsp_rpc.h>
#include <generic_reader.h>
#include <macsim_fmt.h>
#include <tsp_common.h>
#include <tsp_provider_init.h>
