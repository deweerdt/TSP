/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/tests/stage1/Attic/calc_func.c,v 1.1 2002-12-16 18:24:03 dufrenne Exp $

-----------------------------------------------------------------------

Project   : TSP

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
	  tmp = (double)rand()/65535;
	  ret = 10*tmp+mem/10.0; 
	  if (n%20>=10) mem += tmp/100.0; else mem -= tmp/100.0;
	  if (n%500==0) mem = 0;
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
