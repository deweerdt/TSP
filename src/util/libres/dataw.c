/*

$Header: /home/def/zae/tsp/tsp/src/util/libres/dataw.c,v 1.5 2006-10-18 09:58:49 erk Exp $

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
|*								*|
|*   Name	:						            *|
|*								*|
|*	d_wopen		Open data file for write access		            *|
|*	d_wcom		Write comment in data file		            *|
|*	d_wnam		Ecriture du nom et descriptif		            *|
|*			  d'une variable			            *|
|*	d_writ		Ecriture d'un enregistrement		            *|
|*	d_clos		Fermeture du fichier			            *|
|*								*|
|*          version reentrante des fonctions (definir _LIBUTIL_REENTRANT)                       *|
|*	d_wopen_r		Ouvre une fichier res en ecriture		            *|
|*	d_wcom_r		Ecrit les commentaires dans le fichier		*|
|*	d_wnam_r		Ecriture du nom et descriptif		            *|
|*			  d'une variable			            *|
|*	d_writ_r		Ecriture d'un enregistrement		            *|
|*	d_wclos_r		Fermeture du fichier			            *|
|*								*|
|*   Synopsis	:						*|
|*								*|
|*	d_wopen (name);						*|
|*	d_wcom  (com);						*|
|*	d_wnam  (nam,desc);					            *|
|*	d_writ  (rec)						*|
|*	d_clos  ();						            *|
|*								*|
|*	d_wopen_r (name, use_dbl);					*|
|*								*|
|*	int	use_dbl;		Indique si le fichier doit etre un fichier de double ou de float*|
|*	char	*name;		Nom du fichier			*|
|*	char	*com;		Ligne commentaire		            *|
|*	char	*nam;		Nom de variable			*|
|*	char	*desc;		Descriptif de la variable	            *|
|*	float/double *rec;	Tableau de donnees		                        *|
|*								*|
|*   Description :  						            *|
|*								*|
|*	Routines d'interface ecriture avec les fichiers de	                        *|
|*     	donnees							*|
|*								*|
|*								*|
\****************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

#define _LIBUTIL_COMPIL
#include "libUTIL.h"

#ifdef MC

#define	S_IRUSR 0000400		/* read permission, owner */
#define	S_IWUSR 0000200		/* write permission, owner */
#define	S_IXUSR 0000100		/* execute/search permission, owner */
#define	S_IRGRP 0000040		/* read permission, group */
#define	S_IWGRP 0000020		/* write permission, grougroup */
#define	S_IXGRP 0000010		/* execute/search permission, group */
#define	S_IROTH 0000004		/* read permission, other */
#define	S_IWOTH 0000002		/* write permission, other */
#define	S_IXOTH 0000001		/* execute/search permission, other */

#endif

char	*dataw_sccsid={"@(#)dataw.c\t1.12\t00/09/29\tMATRA UTIL"};


struct data_write
{
int	datafd;				/* fichier */
					/* pour d_clos	en lecture	*/
					/* pointeurs sur noms		*/
int	vnum;		/* nombre de variables,comment	*/
				/*           enregistrements	*/
int	recl;			/* Longueur d'un record		*/
int	firstw,firstn;		/* file descrip et flags	*/
int      use_dbl;               /* Utilisation des doubles */

};

typedef struct data_write data_write;
 

extern d_rhandle d_glob_rhandle;

static d_whandle d_glob_whandle = 0;



/*-----------------------------------------------------*\
|							|
|			d_wopen				|
|							|
\*-----------------------------------------------------*/

void	d_wopen (char *name)
{
   d_glob_whandle = d_wopen_r (name, _use_dbl);
  if(!d_glob_whandle)
    {
      exit(1);
    }
}

d_whandle	d_wopen_r (char *name , int use_dbl)
{
	char	nam[MAXPATHLEN];
	data_write* obj;
	obj = (data_write*)calloc(1, sizeof(data_write));
	if(!obj)
	  {
	    fprintf ( stderr, "libRES : *** Echec de l'allocation memoire\n");      
	    return 0;
	  }

	strcpy(nam, name);
	if(strcmp(nam+strlen(nam)-4, ".res") != 0)
		strcat(nam, ".res");

	obj->datafd = open (nam,(O_TRUNC | O_CREAT | O_RDWR),
		(S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH));


	if(obj->datafd<=0) {
		fprintf ( stderr, "*** Erreur a l'ouverture de '%s'\n", nam);
		free(obj);
		return 0;
	}

	obj->firstw = 1;		/*  set first write flag	*/
	obj->firstn = 1;		/*  set first name  flag	*/
	obj->vnum   = 0;		/*  init nombre de var		*/
	obj->use_dbl = use_dbl;         /* use double */

	if(obj->use_dbl)
		write (obj->datafd, "data3", 6);
	else
		write (obj->datafd, "data2", 6);

	return obj;
}

/*-----------------------------------------------------*\
|							|
|			d_wcom				|
|							|
\*-----------------------------------------------------*/
void	d_wcom(char *com)
{
  d_wcom_r(d_glob_whandle, com);
}

void	d_wcom_r(d_whandle h, char *com)
{
  data_write* obj = (data_write*)h;
  
	int	n;

	n = write (obj->datafd,com,strlen(com)+1);

	return;
}

/*-----------------------------------------------------*\
|							|
|			d_wnam				|
|							|
\*-----------------------------------------------------*/
void	d_wnam(char *nam, char *des)
{
  d_wnam_r(d_glob_whandle, nam, des);
}

void	d_wnam_r(d_rhandle h, char *nam, char *des)
{
	int	n;
	data_write* obj = (data_write*)h;
	if (obj->firstn) {				/* si premier appel ecriture	*/
		n  =  write (obj->datafd,"\001",1);	/* pour delimiter comm et noms	*/
		obj->firstn = 0;
	}

	n = write (obj->datafd,nam,strlen(nam)+1);
	n = write (obj->datafd,des,strlen(des)+1);
	obj->vnum++;					/* compeur du nombre de var	*/
}
/*-----------------------------------------------------*\
|							|
|			d_writ				|
|							|
\*-----------------------------------------------------*/
void	d_writ(void *buf)
{
  d_writ_r(d_glob_whandle, buf);
}

void	d_writ_r(d_whandle h, void *buf)
{
	int	n;
	data_write* obj = (data_write*)h;
	void *p;

	if (obj->firstw) {				/* si premier write ecriture	*/
		n = write (obj->datafd,"\001",1);	/* de \001 pour delimiter	*/
		obj->firstw = 0;			/* les zones noms et data	*/
		if(obj->use_dbl)
			obj->recl = obj->vnum * sizeof(double);	/* nombre de char par record	*/
		else
			obj->recl = obj->vnum * sizeof(float);	/* nombre de char par record	*/
	}

	/* FIXME : buf is supposed to be typed to float XOR double
	   shall swap 32 or 64 according to my endianity */
	p=(void *)buf;
	if(obj->use_dbl)
	  for(n=0; n<obj->recl; n+=sizeof(double))
	  {
	    DOUBLE_TO_BE(p);
	    p += sizeof(double);
	  }
	else
	  for(n=0; n<obj->recl; n+=sizeof(float))
	  {
	    FLOAT_TO_BE(p);
	    p += sizeof(float);
	  }

	if ((n = write (obj->datafd,(void *)buf,obj->recl)) != obj->recl) {
		fprintf (stderr,"*** d_writ : Erreur a l'ecriture\n");
		exit(1);
	}

	return;
}

/*-----------------------------------------------------*\
|							|
|			d_clos				|
|							|
\*-----------------------------------------------------*/



void	d_clos()
{

  if(d_glob_rhandle)
    {
      d_rclos_r(d_glob_rhandle);
      d_glob_rhandle = 0;
    }

 if(d_glob_whandle)
    {
      d_wclos_r(d_glob_whandle);
      d_glob_whandle = 0;
    }
	
}

void     d_wclos_r(d_whandle h)
{
   data_write* obj = (data_write*)h;
    close(obj->datafd);
    free(obj);
}
