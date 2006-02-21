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
#include <stdlib.h>
#include <glib.h>
#include "gdisp_pointArray.h"

/*
 * Sets the fields of the structure to default values,
 * And allocate a sample buffer .
 */
DoublePointArray_T*
dparray_newSampleArray (unsigned int maxSamples)
{

  DoublePointArray_T *pArray = (DoublePointArray_T*)NULL;

  pArray = (DoublePointArray_T*)g_malloc0(sizeof(DoublePointArray_T));

  pArray->nbSamples  = 0;
  pArray->current    = 0;
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
  pArray->current = (pArray->current + 1) % pArray->maxSamples;

  /*
   * Check end of ring buffer.
   */
  if (pArray->nbSamples < pArray->maxSamples) {
    pArray->nbSamples++;      
  }

}


/* 
 * Accessors on data.
 */
unsigned int
dparray_getFirstIndex (DoublePointArray_T *pArray)
{

  if (pArray->nbSamples != 0) {
    return (pArray->current)%pArray->nbSamples;      
  }
  else {
    return -1;
  }
 
}

unsigned int
dparray_getCurrentIndex (DoublePointArray_T *pArray)
{

  return pArray->current;

}

unsigned int
dparray_getNbSamples (DoublePointArray_T *pArray)
{

  return pArray->nbSamples;

}


/* 
 * Set/Get on marker, used by upper level to remember a specific position.
 */
unsigned int
dparray_getMarkerIndex (DoublePointArray_T *pArray)
{

  return pArray->marker;

}

void
dparray_setMarkerIndex (DoublePointArray_T *pArray,
			unsigned int               index)
{

  if (pArray->nbSamples != 0) {
    pArray->marker = index % pArray->maxSamples;
  }

}

DoublePoint_T
dparray_getSample (DoublePointArray_T *pArray,
		   int                 index)
{

  if ( (pArray->nbSamples != 0) && (index >= 0) ) {
    return pArray->samples[ index % pArray->nbSamples ]; 
  }
  else {
    return pArray->samples[0]; 
  }

}

DoublePoint_T*
dparray_getSamplePtr (DoublePointArray_T *pArray,
		      int                 index)
{

  if ( (pArray->nbSamples != 0) && (index >= 0) ) {
    return &pArray->samples[ index % pArray->nbSamples ]; 
  }
  else {
    return &pArray->samples[0]; 
  }

}


/*
 * Get the samples available from a specific position
 * in this tricky circular array.
 */
unsigned int 
dparray_getLeftSamplesFromPos (DoublePointArray_T *pArray,
			       unsigned int               index)
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

  printf ("Structure pArray : 0x%X \n", (unsigned int)pArray         );
  printf ("\t ->samples	    : 0x%X \n", (unsigned int)pArray->samples);
  printf ("\t ->nbSamples   : %d   \n", pArray->nbSamples     );
  printf ("\t ->maxSamples  : %d   \n", pArray->maxSamples    );
  printf ("\t ->current     : %d   \n", pArray->current       );
  printf ("\t ->marker	    : %d   \n", pArray->marker        );

}


/*
 * Get back sample values as an array, instead of a structure.
 */
unsigned int
dparray_getFloatTables (DoublePointArray_T  *pArray,
			gfloat             **pXtable,
			gfloat             **pYtable)
{

  unsigned int   startIndex = 0;
  unsigned int   nbPoints   = 0;
  unsigned int   cptPoint   = 0;
  gfloat        *xFloatPtr  = (gfloat*)NULL;
  gfloat        *yFloatPtr  = (gfloat*)NULL;
  DoublePoint_T *pSample    = (DoublePoint_T*)NULL;

  /*
   * Init.
   */
  nbPoints   = dparray_getNbSamples(pArray);
  startIndex = dparray_getFirstIndex(pArray);

  /*
   * Is X table requested ?
   */
  if (pXtable != (gfloat**)NULL) {

    if (*pXtable == (gfloat*)NULL) {
      *pXtable = (gfloat*)g_malloc0(nbPoints * sizeof(gfloat));
    }
    xFloatPtr = *pXtable;

  }

  /*
   * Is Y table requested ?
   */
  if (pYtable != (gfloat**)NULL) {

    if (*pYtable == (gfloat*)NULL) {
      *pYtable = (gfloat*)g_malloc0(nbPoints * sizeof(gfloat));
    }
    yFloatPtr = *pYtable;

  }

  /*
   * Fill the tables.
   */
  for (cptPoint=startIndex; cptPoint<startIndex+nbPoints; cptPoint++) {

    pSample = dparray_getSamplePtr(pArray,cptPoint);

    if (xFloatPtr != (gfloat*)NULL) {
      *xFloatPtr++ = (gfloat)pSample->x;
    }

    if (yFloatPtr != (gfloat*)NULL) {
      *yFloatPtr++ = (gfloat)pSample->y;
    }

  }

  return nbPoints;

}


/*
 * Main program for test purpose.
 */
#ifdef _TEST_POINT_ARRAY
int main(int argc, char *argv[])
{
  int i,nb;
  DoublePointArray_T *pa;
  DoublePoint_T pt,*pp; 
  nb = 10;

  pa = dparray_newSampleArray (nb);
  for (i=0; i<nb; i++) {
    pt.x = i;
    pt.y = 2*i;
    dparray_addSample(pa,&pt);
  }
  
  for (i=dparray_getFirstIndex(pa); i<dparray_getNbSamples(pa); i++) {
    pp = dparray_getSamplePtr(pa,i);
    printf ("pt [%d] => {%f,%f}\n", i, pp->x, pp->y);
  }

  pt.x = i;
  pt.y = 2*i;
  dparray_addSample(pa,&pt);

  printf ("\n");
  for (i=dparray_getFirstIndex(pa); i<dparray_getNbSamples(pa)+dparray_getFirstIndex(pa); i++) {
    pp = dparray_getSamplePtr(pa,i);
    printf ("pt [%d] => {%f,%f}\n", i, pp->x, pp->y);
  }
  return 0;
}
  
#endif
