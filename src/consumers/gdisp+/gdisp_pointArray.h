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

  gdouble x,y;    // Use for storage of physical values.

} DoublePoint_T;

typedef struct _ShortPoint
{

  gshort x,y;	// Use for storage of pixel values.

} ShortPoint_T;
  
typedef struct _DoublePointArray
{

  DoublePoint_T *samples;     // pointer on samples,
  guint          nbSamples;   // number of samples currently in array,
  guint          current;     // index on current position,
  guint          first;       // index on first position,
  guint       	 marker;      // index on a position used by upper level,
  guint          maxSamples;  // maximum samples possible to be added.

} DoublePointArray_T;

/* FIXME : What is ths duplicate struct ?*/
#if 0
typedef struct _DoublePointArrayPtr
{

  DoublePoint_T *samples;     // pointer on samples,
  guint          nbSamples;   // number of samples currently in array,
  guint          current;     // index on current position,
  guint          first;       // index on first position,
  guint       	 marker;      // index on a position used by upper level,
  guint          maxSamples;  // maximum samples possible to be added.

} DoublePointArrayPtr_T;
#endif

/*
 * Return a DoublePoint sample element form an Array and an index.
 */
#define DP_ARRAY_GET_SAMPLE_FROM_FIRST(pArray,i) \
        ((pArray)->samples[ (i + (pArray)->first) % (pArray)->nbSamples ])

#define DP_ARRAY_GET_SAMPLE(pArray,i) \
        ((pArray)->samples[ (i) % (pArray)->nbSamples ])

#define DP_ARRAY_GET_SAMPLE_PTR(pArray,i) \
        (&DP_ARRAY_GET_SAMPLE(pArray,i))

/*
 * Sets the fields to default values and allocate a sample buffer.
 */
DoublePointArray_T *dparray_newSampleArray (guint maxSamples);

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
guint dparray_getFirstIndex   (DoublePointArray_T *pArray);
guint dparray_getCurrentIndex (DoublePointArray_T *pArray);
guint dparray_getNbSamples    (DoublePointArray_T *pArray);

/*
 * Set/Get on marker, used by upper level to remember a specific position.
 */
guint dparray_getMarkerIndex (DoublePointArray_T *pArray);
void  dparray_setMarkerIndex (DoublePointArray_T *pArray, guint index);

/*
 * Get the samples available from specific position in circular array.
 */
guint dparray_getLeftSamplesFromPos (DoublePointArray_T *pArray, guint index);

/*
 * Debug : Dump the values of the pointer.
 */
void dparray_printFields (DoublePointArray_T *pArray);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __POINT_ARRAY_H__ */
