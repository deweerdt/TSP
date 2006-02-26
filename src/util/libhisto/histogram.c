/*

$Header: /home/def/zae/tsp/tsp/src/util/libhisto/histogram.c,v 1.2 2006-02-26 13:36:06 erk Exp $

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



/*----------------------------------------------------------------------+
  | 									|
  |	FILE :		histogram.c					|
  | 									|
  |	DESCRIPTION :	contains functions to build histogram in which	|
  |			values are automatically sorted	and archived	|
  |			according to a paramtizable interval lenght	|
  | 									|
  |     AUTHORS :       Marc LEROY					|
  | 									|
  |	CREATION :	16/06/94			 		|
  | 									|
  |	UPDATES :	20/12/94 Bruno Moisan				|
  |			interval lenght is parametrizable		|
  |			number of intervals is parametrizable		|
  | 									|
  |			1/04/2004 Yves Dufrenne				|
  |			more sample, some comments			|
  | 									|
  +----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
  | 									|
  |	STANDARD INCLUDES						|
  | 									|
  +----------------------------------------------------------------------*/

#include <stdio.h>

/*----------------------------------------------------------------------+
  | 									|
  |	PROJECT INCLUDES						|
  | 									|
  +----------------------------------------------------------------------*/
#include "histogram.h"

/*----------------------------------------------------------------------+
  | 									|
  |	CONSTANTS and MACRO-FUNCTIONS					|
  | 									|
  +----------------------------------------------------------------------*/

#define TSP_HISTO_MAX	1000
#define TSP_HISTO_DELTA	100  /* Default step is 100 us */

/*----------------------------------------------------------------------+
  | 									|
  |	STATIC and GLOBAL VARIABLES					|
  | 									|
  +----------------------------------------------------------------------*/

static 	unsigned long	histo[TSP_HISTO_MAX];

typedef struct {
  double		date;  /* Store the date of the evenement in seconds */
  unsigned long		delta; /* Store the width of this step in us */
} info_evt_t;

/*Use to store when some overrange delta happend */
info_evt_t	list_evt[TSP_HISTO_MAX];

static	unsigned int	max	= TSP_HISTO_MAX;
static	unsigned int	delta	= TSP_HISTO_DELTA;
static  unsigned int	nb_sample;
static  unsigned int	nb_evt;  /* remeber the number of overrange values */
static	double		threshold;

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
  |	MSG_RETURN :	0 if ok, -1 if nb > TSP_HISTO_MAX			|
  | 									|
  +----------------------------------------------------------------------*/

int tsp_histo_nb_interval( unsigned int nb )
{
  if ( (nb >= 0) && (nb <= TSP_HISTO_MAX ) )
    {
      max = nb;
      return 0;
    }
  else
    {
      fprintf (stderr, "WARNING : tsp_histo_nb_interval(%d)	invalid interval number requested", nb);
      return -1;
    }
}

/*----------------------------------------------------------------------+
  | 									|
  |	FUNCTION :	tsp_histo_set_delta					|
  | 									|
  |	DESCRIPTION :	set the width of the intervals			|
  | 									|
  |	PARAMETERS :	width	requested width	in us			|
  | 									|
  |	GLOBAL VAR :	histo, delta				 	|
  | 									|
  |	MSG_RETURN :	0 if ok, -1 if width <= 0			|
  | 									|
  +----------------------------------------------------------------------*/

int tsp_histo_set_delta( unsigned int width )
{
 
  if ( width > 0 )
    {
      delta = width;
      threshold = delta*10;
      return 0;
    }
  else
    {
      fprintf (stderr, "WARNING : tsp_histo_set_delta(%d) invalid interval width", width);
      return -1;
    }
}

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
  |	MSG_RETURN :	none						|
  | 									|
  +----------------------------------------------------------------------*/

void tsp_histo_init()
{
  int i;

  for(i=0;i<max;i++)
    histo[i]=0;

  nb_evt  = nb_sample = 0;
  threshold = delta*10;
}

/*----------------------------------------------------------------------+
  | 									|
  |	FUNCTION :	tsp_histo_enter					|
  | 									|
  |	DESCRIPTION :	outputs the histogram				|
  | 									|
  |	PARAMETERS :	value	value to put in the histogram	in us	|
  | 									|
  |	GLOBAL VAR :	histo, max, delta			 	|
  | 									|
  |	MSG_RETURN :	none						|
  | 									|
  +----------------------------------------------------------------------*/

void tsp_histo_enter( unsigned long value )
{
  if( value < ( max -1 ) * delta )

    histo[value/delta]++;
  else
    histo[max -1]++;
}

void tsp_histo_enter_with_date( unsigned long value, double date_evt )
{
  nb_sample ++;
  tsp_histo_enter (value);

  /* Only try to store value if more than the width step */
  if (value >= threshold && nb_evt<TSP_HISTO_MAX )
    {
      list_evt[nb_evt].date = date_evt;
      list_evt[nb_evt++].delta = value;
    }
}



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
  |	MSG_RETURN :	none						|
  | 									|
  +----------------------------------------------------------------------*/

void tsp_histo_dump( FILE *f, char *title )
{
  int i;
  fprintf(f, "=============== %s ==================\n", title);

  fprintf(f, "DURATION HISTOGRAM (micro seconds) : nb_sample=%d\n", nb_sample);
  for(i=0;i<max -1;i++)
    {
      if (histo[i] !=0)
	fprintf(f,"%10u <= dt < %10u \t: %5ld\n", delta*i, delta*(i+1), histo[i]);
		
    }

  fprintf(f,"             dt >= %4d \t: %5ld\n", delta*(max-1), histo[max-1]);

  /* Guess if witdh is twice bigger than mean , that this evt is interisting */
  if (nb_evt !=0)
    {
      fprintf(f, "DATE Overrange (date in seconde, delta in micro seconds) : nb_evt=%d\n", nb_evt);
      for(i=0;i<nb_evt;i++)
	{
	  fprintf(f,"\t%10.6fs : \t %5lu\n", list_evt[i].date, list_evt[i].delta);
	}
    }

  fprintf(f, "===========================================\n");

}

