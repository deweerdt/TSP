/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/libUTIL.h,v 1.1 2003-01-31 18:32:56 tsp_admin Exp $

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

#ifndef _RES_INTERFACE_H
#define _RES_INTERFACE_H

extern int _use_dbl;

#define RES_COM_NUM	20
#define RES_COM_LEN	256

#define RES_VAR_NUM	3000
#define RES_NAME_LEN	100
#define RES_DESC_LEN	256

typedef void* d_rhandle;
typedef void* d_whandle;



#if defined(_LIBUTIL_REENTRANT) || defined(_LIBUTIL_COMPIL)

d_rhandle d_ropen_r(char *name, int* use_dbl);
int	d_rval_r(d_rhandle h, int c);
void	d_rnam_r(d_rhandle h,char *nam, char *desc, int i);
void	d_rcom_r(d_rhandle h,char *com, int i);
int	d_read_r(d_rhandle h,void *buf);
int	d_dread_r(d_rhandle h,void *buf, int i);
void	d_rclos_r(d_rhandle h);


d_whandle	d_wopen_r(char *name, int use_dbl);
void	d_wcom_r(d_whandle h,char *com);
void	d_wnam_r(d_whandle h,char *nam, char *des);
void	d_writ_r(d_whandle h,void *buf);
void	d_wclos_r(d_whandle h);

#endif

#if !defined(_LIBUTIL_REENTRANT) || defined(_LIBUTIL_COMPIL)

void     d_ropen(char *name);
int	d_rval(int c);
void	d_rnam(char *nam, char *desc, int i);
void	d_rcom(char *com, int i);
int	d_read(void *buf);
int	d_dread(void *buf, int i);

void	d_wopen (char *name);
void	d_wcom(char *com);
void	d_wnam(char *nam, char *des);
void	d_writ(void *buf);

void	d_clos(void);

#endif


#endif /* _RES_INTERFACE_H */
