/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/var_adres.c,v 1.1 2003-01-31 18:32:56 tsp_admin Exp $

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
