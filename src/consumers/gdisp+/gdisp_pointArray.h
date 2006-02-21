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

#ifndef __POINT_ARRAY_H__
#define __POINT_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/*
 * I Could only use an array of Y, with betting,
 * I can compute X with index * FREQ.
 * But I'm not sure of :
 *  - How to take care of data loss (hole in the array ?),
 *  - On the fly changes in sampling,
 *  - Many Variables with differents frequency on the same widget,
 *  => So I use a (x,Y) structure.
 */
     
typedef struct _DoublePoint
{

  double x,y;    /* Use for storage of physical values. */

} DoublePoint_T;

typedef struct _ShortPoint
{

  short x,y;	/* Use for storage of pixel values. */

} ShortPoint_T;
  
typedef struct _DoublePointArray
{

  DoublePoint_T *samples;     /* pointer on samples,                     */
  unsigned int   nbSamples;   /* number of samples currently in array,   */
  unsigned int   current;     /* index on current position,              */
  unsigned int   marker;      /* index on a position used by upper level,*/
  unsigned int   maxSamples;  /* maximum samples possible to be added.   */

} DoublePointArray_T;

/*
 * Return a DoublePoint sample element form an Array and an index.
#define DP_ARRAY_GET_SAMPLE_FROM_FIRST(pArray,i) \
        ((pArray)->samples[ (i + (pArray)->first) % (pArray)->nbSamples ])
*/
DoublePoint_T  dparray_getSample    (DoublePointArray_T *pArray, int index);
DoublePoint_T* dparray_getSamplePtr (DoublePointArray_T *pArray, int index);


/*
 * Sets the fields to default values and allocate a sample buffer.
 */
DoublePointArray_T *dparray_newSampleArray (unsigned int maxSamples);

/*
 * Destroy all memory allocated for this array.
 */
void dparray_freeSampleArray (DoublePointArray_T *pArray);

/*
 * Add an array sample element.
 */
void dparray_addSample (DoublePointArray_T *pArray, DoublePoint_T *pt);

/*
 * Accessors on data.
 */
unsigned int dparray_getFirstIndex   (DoublePointArray_T *pArray);
unsigned int dparray_getCurrentIndex (DoublePointArray_T *pArray);
unsigned int dparray_getNbSamples    (DoublePointArray_T *pArray);

/*
 * Set/Get on marker, used by upper level to remember a specific position.
 */
unsigned int dparray_getMarkerIndex (DoublePointArray_T *pArray);
void  dparray_setMarkerIndex (DoublePointArray_T *pArray, unsigned int index);

/*
 * Get the samples available from specific position in circular array.
 */
unsigned int dparray_getLeftSamplesFromPos (DoublePointArray_T *pArray, unsigned int index);

/*
 * Debug : Dump the values of the pointer.
 */
void dparray_printFields (DoublePointArray_T *pArray);

/*
 * Get back sample values as an array, instead of a structure.
 */
unsigned int dparray_getFloatTables (DoublePointArray_T  *pArray,
				     gfloat             **pXtable,
				     gfloat             **pYtable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __POINT_ARRAY_H__ */
