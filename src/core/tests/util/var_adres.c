#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	*var_adres_sccsid={"@(#)var_adres.c\t1.5\t00/09/29\tMATRA UTIL"};

extern	int	nsym;
extern	char	*((*symadr[100])());
extern	char	symnam[40][20];
extern	char	*tokex();

long	var_adres_ (name,typ,lname,ltyp)
char *name,*typ;
long	lname,ltyp;
{
char	unit[80];
char	module[16];
int	off,i;
char	*p;

p = tokex ( name, module, &off);
if (p == NULL) {
	return(0L);
	}

for (i=0;i<nsym;i++) {
	if(strcmp(&symnam[i][0],module)==0)
		break;
	}
if (i == nsym){
	return (0L);
	}
return ((long)(*symadr[i])(name,typ,unit,(long)strlen(name),ltyp,80L));
}
