#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unit.h"
#include "dunit.h"

char	*unit_sccsid={"@(#)unit.c\t1.4\t00/09/29\tMATRA UTIL"};

struct sunit si_unit (char *unit)
{
	char		*units;
	char		*cval,chval[50],c;
	int		i,k,inv,ordre;
	struct sunit	out;
	double		conv = 1.;

	inv = 1;
	units = unit;
	cval = chval;
	while (*unit == ' ')
		unit++;
	if (*unit=='-' || *unit=='+' || (*unit<='9'&& *unit>='0') || *unit =='.') {
		while ((*unit=='-' || *unit=='+' || (*unit<='9' && *unit>='0')
			|| *unit =='.' || *unit=='e' || *unit=='E' ) && cval <= &chval[48])
			*cval++ = *unit++;
	}
	if (cval != chval) {
		*cval='\0';
		sscanf(chval, "%lf", &conv);
	}

	while (*unit==' ')
		unit++;

	for (i=0;i<6;i++)
		out.d.t[i] = 0;

	for(;;) {

		cval = chval;
		ordre = 1;

		if (*unit == '\0')
			break;

		while(*unit!='-' && *unit !='/' && *unit != '\0' && *unit != ' ')
			*cval++ = *unit++;

		*cval='\0';

		if ( (c= *(cval-1))<='9' && c >='0') {
			ordre = c - '0';
			*(cval-1) ='\0';
		}

		for (i=0;i<NUNIT;i++) {
			if(strcmp(chval,dic[i].name)==0) {
				add_dim(&(out.d),dic[i].dim,inv*ordre);
				for (k=0;k<ordre;k++) {
					if (inv==1)
						conv = conv *  dic[i].fact;
					else
						conv = conv/dic[i].fact;
				}
				break;
			}
		}

		if (i==NUNIT) {
			fprintf (stderr,"*** unit : illegal unit %s\n",units);
			exit(1);
		}

		if (*unit=='/')
			inv = -1;

		if (*unit=='\0' || *unit==' ')
			break;

		unit++;
	}

	out.conv = conv;
	return(out);
}

int add_dim (struct dim *a, struct dim *b, int ordr)
{
	int	i;

	for (i=0;i<6;i++)
		a->t[i] += b->t[i] * ordr;
}

int cmp_unit (struct dim *a, struct dim *b)
{
	int	i;
	int	flga,flgb,flgd;

	flga = flgb = flgd = 0;

	for (i=0;i<6;i++) {
		if (a->t[i] != 0 )
			flga = 1;
		if (b->t[i] != 0 )
			flgb = 1;
		if (a->t[i]!=b->t[i])
			flgd = 1;
	}

	if (flga==1 && flgb==1)
		return (flgd);

	return (0);
}

