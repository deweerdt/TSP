/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <glib.h>
#include "gdisp_pointArray.h"

/*
 * Sets the fields of the structure to default values,
 * And allocate a sample buffer .
 */
DoublePointArray_T*
dparray_newSampleArray (guint maxSamples)
{

  DoublePointArray_T *pArray = (DoublePointArray_T*)NULL;

  pArray = (DoublePointArray_T*)g_malloc0(sizeof(DoublePointArray_T));

  pArray->nbSamples  = 0;
  pArray->current    = 0;
  pArray->first      = 0;
  pArray->marker     = 0;
  pArray->maxSamples = maxSamples;
  pArray->samples    = (DoublePoint_T*)
                       g_malloc0(maxSamples * sizeof(DoublePoint_T));

  return pArray;

}


/*
 * Free memory that has been allocated to this point array.
 */
void
dparray_freeSampleArray (DoublePointArray_T* pArray)
{

  g_free (pArray->samples);
  g_free (pArray);

}


/*
 * Add an array sample element.
 */
void
dparray_addSample (DoublePointArray_T *pArray,
		   DoublePoint_T      *point)
{
  pArray->samples[pArray->current] = *point;
  pArray->current = (pArray->current + 1 ) % pArray->maxSamples;

  /*
   * Check end of ring buffer.
   */
  if (pArray->nbSamples != pArray->maxSamples) {

    pArray->nbSamples++;      
    pArray->first = 0;

  }
  else {

    pArray->first = pArray->current;

  }

}


/* 
 * Accessors on data.
 */
guint
dparray_getFirstIndex (DoublePointArray_T *pArray)
{

  return pArray->first;

}

guint
dparray_getCurrentIndex (DoublePointArray_T *pArray)
{

  return pArray->current;

}

guint
dparray_getNbSamples (DoublePointArray_T *pArray)
{

  return pArray->nbSamples;

}


/* 
 * Set/Get on marker, used by upper level to remember a specific position.
 */
guint
dparray_getMarkerIndex (DoublePointArray_T *pArray)
{

  return pArray->marker;

}

void
dparray_setMarkerIndex (DoublePointArray_T *pArray,
			guint               index)
{

  pArray->marker = index % pArray->maxSamples;

}

DoublePoint_T
DP_ARRAY_GET_SAMPLE (DoublePointArray_T *pArray, int i)
{

  if (i >= 0)
    return pArray->samples[ i % pArray->nbSamples ]; 
  else
    return pArray->samples[0]; 

}

DoublePoint_T*
DP_ARRAY_GET_SAMPLE_PTR (DoublePointArray_T *pArray, int i)
{

  if (i >= 0)
    return &pArray->samples[ i % pArray->nbSamples ]; 
  else
    return &pArray->samples[0]; 

}


/*
 * Get the samples available from a specific position
 * in this tricky circular array.
 */
guint 
dparray_getLeftSamplesFromPos (DoublePointArray_T *pArray,
			       guint               index)
{

  if (index < pArray->current) {

    return pArray->current - index;

  }
  if (index > pArray->current) {

    return pArray->current + pArray->maxSamples - index;

  }

  return pArray->nbSamples;

}


/*
 * Debug : Dump the values of the pointer.
 */
void
dparray_printFields (DoublePointArray_T *pArray)
{

  printf ("Structure pArray : 0x%X \n", (guint)pArray         );
  printf ("\t ->samples	    : 0x%X \n", (guint)pArray->samples);
  printf ("\t ->nbSamples   : %d   \n", pArray->nbSamples     );
  printf ("\t ->maxSamples  : %d   \n", pArray->maxSamples    );
  printf ("\t ->current     : %d   \n", pArray->current       );
  printf ("\t ->first	    : %d   \n", pArray->first         );
  printf ("\t ->marker	    : %d   \n", pArray->marker        );

}
