/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/dataf.c,v 1.2 2002-12-18 16:27:46 tntdev Exp $

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

char	*dataf_sccsid={"@(#)dataf.c\t1.7\t00/09/29\tMATRA UTIL"};

#include  <stdio.h>
#include  <sys/param.h>
#include  "libUTIL.h"


void	d_wopen_(name, lname)
char	*name;
long	lname;
{
	char	ncop[MAXPATHLEN];

	strncpy ( ncop, name, (int)lname);
	ncop [ (int)lname ] = '\0';
	d_wopen(ncop);
}


void	d_ropen_(name, lname)
char	*name;
long	lname;
{
	char	ncop[MAXPATHLEN];

	strncpy ( ncop, name, (int)lname);
	ncop [ (int)lname ] = '\0';
	d_ropen(ncop);
}

void	d_wcom_(name, lname)
char	*name;
long	lname;
{
	char	ncop[RES_COM_LEN];

	strncpy ( ncop, name, (int)lname);
	ncop [ (int)lname ] = '\0';
	d_wcom(ncop);
}

void	d_rcom_(name, nc, lname)
char	*name;
int	*nc;
long	lname;
{
	char	ncop[RES_COM_LEN];
	int	i;

	d_rcom(ncop,*nc-1);
	for (i=0; ncop[i] != '\0' ; i++ )
		name [i] = ncop[i];
	for (; i< (int)lname;i++)
		name [i] = ' ';
}

void	d_wnam_(name,desc, lname, ldesc)
char	*name,*desc;
long	lname,ldesc;
{
	char	ncop[RES_NAME_LEN],
		dcop[RES_DESC_LEN];

	strncpy ( ncop, name, (int)lname);
	ncop [ (int)lname ] = '\0';
	strncpy ( dcop, desc, (int)ldesc);
	dcop [ (int)ldesc ] = '\0';
	d_wnam(ncop,dcop);
}

void	d_rnam_(name,desc, ni,lname,ldesc)
char	*name,*desc;
int	*ni;
long	lname,ldesc;
{
	char	ncop[RES_NAME_LEN],
		dcop[RES_DESC_LEN];
	int	i;

	d_rnam(ncop,dcop,*ni - 1);
	for (i=0; ncop[i] != '\0' ; i++ )
		name [i] = ncop[i];
	for (; i< (int)lname;i++)
		name [i] = ' ';
	for (i=0; dcop[i] != '\0' ; i++ )
		desc [i] = dcop[i];
	for (; i< (int)ldesc;i++)
		desc [i] = ' ';
}

d_clos_()
{
	d_clos();
}

int	d_rval_ (typ,val)
char	*typ;
int	*val;
{
	*val = d_rval(*typ);
}


d_writ_ ( buf)
float	*buf;
{
	d_writ (buf);
}

d_read_ (buf,ierr)
float	*buf;
int	*ierr;
{
	if (d_read (buf)== EOF)
		*ierr = -1;
}

