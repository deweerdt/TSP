/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/mdatar.c,v 1.2 2003-02-03 17:40:13 tsp_admin Exp $

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

/****************************************************************\
|*   Description :  						*|
|*								*|
|*	Routines d'interface lecture avec les fichiers de	*|
|*     	donnees							*|
|*								*|
\****************************************************************/

#include  <stdlib.h>
#include  <unistd.h>
#include  <math.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <string.h>
#include  <assert.h>
#include  <sys/types.h>
#include  <sys/mman.h>
#include  <sys/param.h>

#include  "mdatar.h"

/*-----------------------------------------------------*\
|							|
|			md_ropen			|
|							|
\*-----------------------------------------------------*/
RES_HANDLE *md_ropen(char *name)
{
	int		fd;
	int		zone;
	char		*cptr;
	void		*pa, *ptmp;
	int		len;
	int		datalen;
	RES_HANDLE	*ha;
	int		kvar;

/* Allocation du RES_HANDLE */
/*--------------------------*/
	ha = (RES_HANDLE *)calloc(1, sizeof(RES_HANDLE));
	if(ha == (RES_HANDLE *)0)
		return (RES_HANDLE *)0;

/* Rajout si necessaire de l'extension .res */
/*------------------------------------------*/
	ha->filename = malloc(strlen(name)+4+1);
	if(ha->filename == (char *)0) {
		free(ha);
		return(RES_HANDLE *)0;
	}
	strcpy(ha->filename, name);
	if(strcmp(ha->filename+strlen(ha->filename)-4, ".res") != 0)
		strcat(ha->filename, ".res");

	fd = open(ha->filename, O_RDONLY);
	if(fd <=0 ) {
		free(ha);
		return (RES_HANDLE *)0;
	}

/* Lecture taille totale du fichier */
/*----------------------------------*/
	len = lseek(fd,0L,2);	/*  Taille du fichier */

/* Mapping du fichier dans l'espace du process (RDONLY) */
/*------------------------------------------------------*/
	pa = (void *)mmap(NULL, len, PROT_READ, MAP_SHARED|MAP_NORESERVE, fd, 0L);
	if(pa == MAP_FAILED) {
		free(ha);
		return (RES_HANDLE *)0;
	}
	ha->pmmap = pa;
	ha->lmmap = len;

/* Fermeture du fichier : acces mappes seulement ... */
/*---------------------------------------------------*/
	close(fd);

/* Lecture du champ version format */
/* ( s'il existe : 6 caracteres )  */
/*---------------------------------*/
	cptr = pa;

	if(strncmp(cptr, "data3", 6)==0) {
		ha->use_dbl = 1;
		ha->version = 2;
		cptr+=6;
	}
	if(strncmp(cptr, "data2", 6)==0) {
		ha->use_dbl = 0;
		ha->version = 1;
		cptr+=6;
	}
	else {
		ha->use_dbl = 0;
		ha->version = 0;
	}

/* Lecture de l'entete du fichier */
/*--------------------------------*/
	zone = ZONE_COMMENT;		/* indicateur de zone en cours		*/
	ha->nb_com = 0;
	ha->pcomm[ha->nb_com++] = cptr;

	while (zone != ZONE_DATA) {	/* Boucle de lecture jusqu'a la zone	*/
					/*  de donnees				*/

	switch (*cptr++)	{	/* Test du caratere lu ...		*/

	case '\0' :		/* ... fin de chaine de caractere	*/

		switch (zone) {

		case ZONE_COMMENT :
			if(*cptr!='\1')
				ha->pcomm[ha->nb_com++] = cptr;
			break;

		case ZONE_VARIABLE :
			if(*cptr!='\1') {
				if( (kvar++ % 2)==0 )
					ha->pnvar[ha->nb_var] = cptr;
				else
					ha->pcvar[ha->nb_var++] = cptr;
			}
			break;
		}
		break;

	case '\1' :		/* ... fin de zone			*/

		switch (zone) {

		case ZONE_COMMENT :
			zone = ZONE_VARIABLE;
			ha->nb_var = 0;
			ha->pnvar[ha->nb_var] = cptr;
			kvar = 1;
			break;

		case ZONE_VARIABLE :
			zone = ZONE_DATA;
			if((kvar% 2)==1)
				fprintf(stderr, "WARNING : description de variable incomplete\n");

			ha->pdat = (float *)cptr;
			break;
		}
		break;

	case '\2' :		/* ... remplissage pour alignement 	*/
		break;
	}

	}

	datalen = len-((char *)cptr-(char *)pa);
	if(ha->use_dbl) {
		ha->nb_rec = datalen/(ha->nb_var*sizeof(double));
		if(datalen%(ha->nb_var*sizeof(double)) != 0)
			fprintf(stderr, "WARNING : %d isolated bytes at the end of the file\n",
				datalen%(ha->nb_var*sizeof(double)) );
	}
	else {
		ha->nb_rec = datalen/(ha->nb_var*sizeof(float));
		if(datalen%(ha->nb_var*sizeof(float)) != 0)
			fprintf(stderr, "WARNING : %d isolated bytes at the end of the file\n",
				datalen%(ha->nb_var*sizeof(float)) );
	}

	/* realloc data pointer for avoiding unaligned access */
	ptmp = malloc(datalen);
	assert(ptmp!=(void*)0);
	memcpy(ptmp,ha->pdat,datalen);
	ha->pdat=ptmp;
	 
	return ha;
}

void md_rclose(RES_HANDLE *h1)
{
	if((h1->pmmap != 0) && (h1->pmmap != MAP_FAILED))
		munmap(h1->pmmap, h1->lmmap);

	free(h1->pdat);
	free(h1);

	return;
}

void info_show(RES_HANDLE *h1)
{
	int	i;

	printf("format version %d\n", h1->version);
	printf("nb_com = %d\n", RES_NB_COM(h1));
	printf("nb_var = %d\n", RES_NB_VAR(h1));
	printf("nb_rec = %d\n", RES_NB_REC(h1));
	printf("t0 = %g\n", *RES_RECORD(h1,0));
	printf("tM = %g\n", *RES_RECORD(h1,RES_NB_REC(h1)-1));
	for(i=0;i<RES_NB_COM(h1);i++) {
		printf("COMM[%d] = %s\n", i, h1->pcomm[i]);
	}
	for(i=0;i<RES_NB_VAR(h1);i++) {
		printf("VARN[%02d] = %s\n", i, h1->pnvar[i]);
		printf("VARC[%02d] = %s\n", i, h1->pcvar[i]);
	}
}

