/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/mdatar.h,v 1.2 2002-12-18 16:27:48 tntdev Exp $

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

#ifndef __MDATAR_H
#define __MDATAR_H

/****************************************************************\
|*								*|
|*   Description :  						*|
|*								*|
|*	Routines d'interface lecture avec les fichiers de	*|
|*     	donnees							*|
|*								*|
\****************************************************************/

#define NB_MAX_FILES   16
#define NB_MAX_COMMENT   256
#define NB_MAX_VARIABLES 2048

typedef struct {

	int	version;		/* 0 par defaut			*/
					/* 1 si data2,			*/
					/* 2 si data3			*/
	int	use_dbl;		/* O : float, 1 : double        */
	char	*filename;		/* nom du fichier		*/
	char	*pmmap;			/* pointeur sur zone mmappee	*/
	int	lmmap;			/* longueur zone mmappee	*/
	char	*pcomm[NB_MAX_COMMENT];	/* pointeurs sur commentaires	*/
	char	*pnvar[NB_MAX_VARIABLES];
	char	*pcvar[NB_MAX_VARIABLES];
	float 	*pdat;			/* pointeur sur le debut des	*/
					/* donnees			*/
	int	nb_var;			/* Nombre de variables		*/
	int	nb_com;			/* Nombrte de commentaires	*/
	int	nb_rec;			/* nombre d'enregistrements	*/

} RES_HANDLE;

#define RES_FILENAME(ha) ((ha)->filename)
#define RES_COMMENT(ha, i) ((ha)->pcomm[(i)])
#define RES_VARNAME(ha, i) ((ha)->pnvar[(i)])
#define RES_VARCOM(ha, i) ((ha)->pcvar[(i)])
#define RES_NB_VAR(ha) ((ha)->nb_var)
#define RES_NB_COM(ha) ((ha)->nb_com)
#define RES_NB_REC(ha) ((ha)->nb_rec)
#define RES_RECORD(ha, indrec) ((float *)(ha)->pdat + (ha)->nb_var*(indrec))

enum { ZONE_COMMENT, ZONE_VARIABLE, ZONE_DATA };

/* Prototypes */
extern RES_HANDLE	*md_ropen(char *name);
extern void		md_rclose(RES_HANDLE *h1);
extern void		info_show(RES_HANDLE *h1);


#endif /* __MDATAR_H */
