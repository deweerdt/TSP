/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/iadr.c,v 1.2 2002-12-18 16:27:47 tntdev Exp $

-----------------------------------------------------------------------

libUTIL - core components to read and write res files

Copyright (c) 2002 Marc LE ROY 

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
Maintainer : tsp@astrium-space.com
Component : libUTIL

-----------------------------------------------------------------------

Purpose   :

-----------------------------------------------------------------------
 */

char	*iadr_sccsid={"@(#)iadr.c\t1.4\t00/09/29\tMATRA UTIL"};

char	*iadr_(a)
char	*a;
{
return(a);
}

double get_float_(i)
double **i;
{
union hh {
	double a;
	float b[2];
	} hh;
hh.b[0] = *(float *)*i;
return ( hh.a);
}
double get_double_(i)
double **i;
{
return ( *(double *)*i);
}
put_float_(i,f)
float	**i;
float	*f;
{
**i = *f;
}
put_double_(i,f)
double	**i;
double	*f;
{
**i = *f;
}
put_long_(i,l)
long	**i,*l;
{
**i = *l;
}
put_short_(i,s)
short	**i,*s;
{
**i = *s;
}
long get_long_(i)
long **i;
{
return ( *(long *)*i);
}
short get_short_(i)
short **i;
{
return ( *(short *)*i);
}

