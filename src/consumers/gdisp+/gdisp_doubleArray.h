/*!  \file 

$Id: gdisp_doubleArray.h,v 1.1 2004-02-04 20:32:09 esteban Exp $

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

#ifndef __DOUBLE_ARRAY_H__
#define __DOUBLE_ARRAY_H__

/*
 * samples    : pointer on samples,
 * nbSamples  : number of samples currently in array,
 * current    : index on current position,
 * first      : index on first position,
 * maxSamples : maximum samples possible to be added.
 */
typedef struct DoubleArray_T_
{

  gdouble  *samples;
  guint     nbSamples;
  guint     current;
  guint     first;
  guint     maxSamples;

} DoubleArray_T;


/*
 * Return a double sample element form an Array and an index.
 */
#define DA_GET_SAMPLE_FROM_FIRST(pArray,i) \
        ((pArray)->samples[ (i + (pArray)->first) % (pArray)->nbSamples ])

#define DA_GET_SAMPLE(pArray,i) \
        ((pArray)->samples[ (i) % (pArray)->nbSamples ])

#define DA_GET_SAMPLE_PTR(pArray,i) \
        (&DA_GET_SAMPLE(pArray,i))

/*
 * Sets the fields to default values and allocate a sample buffer.
 */
DoubleArray_T *da_newSampleArray (guint maxSamples);

/*
 * Destroy all memory allocated for this array.
 */
void da_freeSampleArray (DoubleArray_T *dArray);

/*
 * Add an array sample element.
 */
void da_addSample (DoubleArray_T *dArray, gdouble dValue);

/*
 * Accessors on data.
 */
guint da_getFirstIndex   (DoubleArray_T *dArray);
guint da_getCurrentIndex (DoubleArray_T *dArray);
guint da_getNbSamples    (DoubleArray_T *dArray);

/*
 * Get the samples available from specific position in circular array.
 */
guint da_getLeftSamplesFromPos (DoubleArray_T *dArray, guint index);

/*
 * Debug : Dump the values of the pointer.
 */
void darray_printFields (DoubleArray_T *dArray);

#endif /* __DOUBLE_ARRAY_H__ */
