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

