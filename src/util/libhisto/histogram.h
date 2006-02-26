/*

$Header: /home/def/zae/tsp/tsp/src/util/libhisto/histogram.h,v 1.2 2006-02-26 13:36:06 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

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
Maintainer: tsp@astrium.eads.net
Component : Provider

-----------------------------------------------------------------------
*/

#ifndef _H_HISTO
#define _H_HISTO


/*----------------------------------------------------------------------+
 | 									|
 |	FILE :		histogram.c					|
 | 									|
 |	DESCRIPTION :	contains functions to build histogram in which	|
 |			values are automatically sorted	and archived	|
 |			according to a paramtizable interval lenght	|
 | 									|
 |      AUTHORS :       Marc LEROY					|
 | 									|
 |	CREATION :	16/06/94			 		|
 | 									|
 |	UPDATES :	20/12/94 Bruno Moisan				|
 |			interval lenght is parametrizable		|
 |			number of intervals is parametrizable		|
 | 									|
 +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
 | 									|
 |	FUNCTION :	tsp_histo_nb_interval				|
 | 									|
 |	DESCRIPTION :	set the number of used intervals		|
 | 									|
 |	PARAMETERS :	nb	number of requested interval		|
 | 									|
 |	GLOBAL VAR :	histo, max				 	|
 | 									|
 |	RETURN :	0 if ok, -1 if nb > TSP_HISTO_MAX			|
 | 									|
 +----------------------------------------------------------------------*/

int tsp_histo_nb_interval( unsigned int nb );

/*----------------------------------------------------------------------+
 | 									|
 |	FUNCTION :	tsp_histo_set_delta					|
 | 									|
 |	DESCRIPTION :	set the width of the intervals			|
 | 									|
 |	PARAMETERS :	width	requested width				|
 | 									|
 |	GLOBAL VAR :	histo, delta				 	|
 | 									|
 |	RETURN :	0 if ok, -1 if width <= 0			|
 | 									|
 +----------------------------------------------------------------------*/

int tsp_histo_set_delta( unsigned int width /*us*/);

/*----------------------------------------------------------------------+
 | 									|
 |	FUNCTION :	tsp_histo_init					|
 | 									|
 |	DESCRIPTION :	clear the histogram				|
 | 									|
 |	PARAMETERS :	none						|
 | 									|
 |	GLOBAL VAR :	histo, max				 	|
 | 									|
 |	RETURN :	none						|
 | 									|
 +----------------------------------------------------------------------*/

void tsp_histo_init(void);

/*----------------------------------------------------------------------+
 | 									|
 |	FUNCTION :	tsp_histo_enter					|
 | 									|
 |	DESCRIPTION :	outputs the histogram				|
 | 									|
 |	PARAMETERS :	value	value to put in the histogram		|
 | 									|
 |	GLOBAL VAR :	histo, max, delta			 	|
 | 									|
 |	RETURN :	none						|
 | 									|
 +----------------------------------------------------------------------*/

void tsp_histo_enter( unsigned long value /*us*/);
void tsp_histo_enter_with_date( unsigned long value, double date_evt );



/*----------------------------------------------------------------------+
 | 									|
 |	FUNCTION :	tsp_histo_dump					|
 | 									|
 |	DESCRIPTION :	outputs the histogram				|
 | 									|
 |	PARAMETERS :	f	stream on which histogram has to be	|
 |				displayed				|
 | 									|
 |			title	title of the histogram			|
 | 									|
 |	GLOBAL VAR :	histo, max, delta			 	|
 | 									|
 |	RETURN :	none						|
 | 									|
 +----------------------------------------------------------------------*/

void tsp_histo_dump( FILE *f, char *title );

#endif

