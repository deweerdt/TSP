/*

$Id: gdisp_doubleArray.c,v 1.3 2006-02-26 14:08:23 erk Exp $

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

File      : Management of an array of double value.

-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <glib.h>
#include "gdisp_doubleArray.h"

/*
 * Sets the fields of the structure to default values,
 * And allocate a sample buffer .
 */
DoubleArray_T*
da_newSampleArray (guint maxSamples)
{

  DoubleArray_T *dArray = (DoubleArray_T*)NULL;

  dArray = (DoubleArray_T*)g_malloc0(sizeof(DoubleArray_T));

  dArray->nbSamples  = 0;
  dArray->current    = 0;
  dArray->first      = 0;
  dArray->maxSamples = maxSamples;
  dArray->samples    = (gdouble*)g_malloc0(maxSamples * sizeof(gdouble));

  return dArray;

}


/*
 * Free memory that has been allocated to this point array.
 */
void
da_freeSampleArray (DoubleArray_T* dArray)
{

  g_free (dArray->samples);
  g_free (dArray);

}


/*
 * Add an array sample element.
 */
void
da_addSample (DoubleArray_T *dArray,
	      gdouble        dValue)
{

  dArray->samples[dArray->current] = dValue;
  dArray->current = (dArray->current + 1 ) % dArray->maxSamples;

  /*
   * Check end of ring buffer.
   */
  if (dArray->nbSamples != dArray->maxSamples) {

    dArray->nbSamples++;      
    dArray->first = 0;

  }
  else {

    dArray->first = dArray->current;

  }

}


/* 
 * Accessors on data.
 */
guint
da_getFirstIndex (DoubleArray_T *dArray)
{

  return dArray->first;

}

guint
da_getCurrentIndex (DoubleArray_T *dArray)
{

  return dArray->current;

}

guint
da_getNbSamples (DoubleArray_T *dArray)
{

  return dArray->nbSamples;

}


/*
 * Get the samples available from a specific position
 * in this tricky circular array.
 */
guint 
da_getLeftSamplesFromPos (DoubleArray_T *dArray,
			  guint          index)
{

  if (index < dArray->current) {

    return dArray->current - index;

  }
  if (index > dArray->current) {

    return dArray->current + dArray->maxSamples - index;

  }

  return dArray->nbSamples;

}


/*
 * Debug : Dump the values of the pointer.
 */
void
da_printFields (DoubleArray_T *dArray)
{

  printf ("Structure dArray : 0x%X \n", (guint)dArray         );
  printf ("\t ->samples	    : 0x%X \n", (guint)dArray->samples);
  printf ("\t ->nbSampl	    : %d   \n", dArray->nbSamples     );
  printf ("\t ->maxSample   : %d   \n", dArray->maxSamples    );
  printf ("\t ->current     : %d   \n", dArray->current       );
  printf ("\t ->first	    : %d   \n", dArray->first         );

}
