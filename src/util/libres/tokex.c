/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/tokex.c,v 1.1 2003-01-31 18:32:56 tsp_admin Exp $

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

char	*tokex_sccsid={"@(#)tokex.c\t1.5\t00/09/29\tMATRA UTIL"};

char	*tokex (poi,name,off)
char	*poi,*name;
int	*off;
{
int	i;
char	delim,arg[11];

i = 0;
*off = 0;
while ( *poi != '[' && *poi != '(' && *poi != '.' && *poi != ' ' && *poi != '\0' && i++ < 40 ) 
	*name++ = *poi++;
*name++ = '\0';
if (i==41 || *poi == '\0')
	return(NULL);

if ( *poi == '[' || *poi == '('){
	i = 0;
	if (*poi == '[')
		delim = ']';
	else
		delim = ')';
	poi++;
	while (*poi != delim && i <10 )
		arg[i++] = *poi++;
	arg[i] = '\0';
	poi++;
	if(sscanf (arg,"%d",off)!=1)
		return(NULL);
	if(delim == ')')
		*off = *off -1;

}
if (*poi == '.')
	poi++;
if (*poi == '\0'||*poi==' ')
	return(NULL);
else
	return(poi);
}
char	*nscan ( name, off, unit)
char	*name, **unit;
int	*off;
{
/* fonction permettant d'extraire le dernier champ d'offset */
/* et l'unite						    */
/*----------------------------------------------------------*/

char	*p,*ps,rdelim,ldelim;

p = name + strlen(name) -1;

if ( *p == '}' ) {
	*p = '\0';
	while (*p != '{' && (p > name))
		p--;
	*p = '\0';
	*unit = p+1;
	p--;
	}
else {
	*unit = p+1;
	}
if ( p < name) {
	*off = 0;
	return (p);
	}
rdelim = *p;
if (rdelim == ')')
	ldelim = '(';
else if (rdelim == ']')
	ldelim = '[';
else
	rdelim = '\0';

if (rdelim != '\0') {
	ps = p;
	*ps = '\0';
	while (*p != ldelim && (p > name))
		p--;
	if (sscanf(p+1,"%d",off)!=1)
		return ( NULL);
	*ps = rdelim;
	if (ldelim == '(')
		*off -= 1;
	p--;
	}
else {
	*off = 0;
	}
return (p);
}
long	tokex_ (poi,name,off,lpoi,lname)
char	*poi,*name;
long	*off,lpoi,lname;
{
char	*p1;
int	i;
p1 = tokex ( poi, name, (int *)off);
for (i=strlen(name);i<lname;i++)
	name[i] =' ';
if (p1!=NULL)
return ((long)(p1-poi)+1);
else
return (0L);
}
