/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libcalc/calc_func.c,v 1.1 2003-01-31 18:32:56 tsp_admin Exp $

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
Maintainer : tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for demo purpose

-----------------------------------------------------------------------
 */


#include <math.h>


/* return nice value for plotting */
double calc_func (int index, double my_time)
{
  double ret=0;
  static double mem=0;
  if ( index == 0)
    ret = index;
  else
    {
      int module = index % 10;
      double tmp, t = (my_time + index)/100.0;
      int n = (int)t;
      double r = t-n;

      switch (module)
	{
	case 0: 
	  ret = cos (t); 
	  break;
	case 1: 
	  ret = sin (t); 
	  break;
	case 2: 
	  /* FIXME : the rand function is not MT-safe for SUN */
	  /*tmp = (double)rand()/65535;
	  ret = 10*tmp+mem/10.0; 
	  if (n%20>=10) mem += tmp/100.0; else mem -= tmp/100.0;
	  if (n%500==0) mem = 0;*/
	  ret = sin(t);
	  ret = ( ret > 0 ? ret : 1+ret); 
	  break;
	case 3:  
	  ret = log (n%10+r+1); 
	  break;
	case 4: 
	  ret = exp (r+(double)(n%10)); 
	  break;
	case 5: 
	  ret = module*(n%20-10)*r; 
	  break;
	case 6: 
	  ret = cos(t)* (n%100); 
	  break;
	case 7: 
	  ret = 100*(n%10); 
	  break;
	case 8: 
	  if (n%10>=5)
	    ret = 1;
	  else
	    ret = -1;
	  break;
	case 9: 
	  ret = sin(t)*(n%20)+cos(t)*(n%20); 
	  break;
	default:
	  ret = -1;
	}
    }
  return ret;
}	    
