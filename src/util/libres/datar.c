/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/datar.c,v 1.5 2004-09-07 09:27:45 tractobob Exp $

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
Maintainer: tsp@astrium-space.com
Component : libUTIL

-----------------------------------------------------------------------

Purpose   :

-----------------------------------------------------------------------
 */

/****************************************************************\
|*								*|
|*   Name	:						            *|
|*								*|
|*	d_ropen		Ouverture pour lecture			            *|
|*	d_rcom		Lecture d'une ligne commentaire	 	            *|
|*	d_rnam		Lecture d'une declaration		            *|
|*	d_read		Lecture d'un enregistrement		            *|
|*	d_dread		Lecture a acces aleatoire		            *|
|*	d_rval		Retourne des parametres de taille	                        *|
|*							            *|
|*          version reentrante des fonctions (definir _LIBUTIL_REENTRANT)                       *|
|*	d_ropen_r		Ouverture pour lecture			            *|
|*	d_rcom_r		Lecture d'une ligne commentaire		            *|
|*	d_rnam_r		Lecture d'une declaration		            *|
|*	d_read_r		Lecture d'un enregistrement		            *|
|*	d_dread_r		Lecture a acces aleatoire		            *|
|*	d_rval_r		Retourne des parametres de taille	                        *|
|*	d_rclos_r		Fermeture apres lecture                                     *|
|*								*|
|*								*|
|*   Synopsis	:						*|
|*								*|
|*								*|
|*	d_ropen (name);						*|
|*	d_rcom  (com,i);					            *|
|*	d_rnam  (nam,desc,i);					            *|
|*	n  =  d_read  (rec);					            *|
|*	n  =  d_dread (rec,i);				            	*|
|*	v  =  d_rval ( c);					            *|
|*								*|
|*	d_ropen_r (name, use_dbl);					*|
|*								*|
|*	int	use_dbl;		Indique si le fichier est un fichier de double ou de float*|
|*	char	*name;		Nom du fichier			*|
|*	char	*com;		Ligne commentaire		            *|
|*	char	*nam;		Nom de variable			*|
|*	char	*desc;		Descriptif de la variable	            *|
|*	float/double *rec;	Tableau de donnees		                        *|
|*	int	i;		Numero de l'enregistrement	            *|
|*	int	c;		Type de valeur a retourner :	            *|
|*				  'v'  nombre de variables	            *|
|*				  'r'  nombre d'enregistrements                 *|
|*				  'c'  nombre de commentaires	            *|
|*	int	v;		Valeur retournee		            *|
|*	int	n;		Nombre de char lus ou EOF	            *|
|*								*|
|*   Description :  						            *|
|*								*|
|*	Routines d'interface lecture avec les fichiers de	                        *|
|*     	donnees							*|
|*								*|
\****************************************************************/
#include  <fcntl.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/param.h>

#define _LIBUTIL_COMPIL
#include "libUTIL.h"

char	*datar_sccsid={"@(#)datar.c\t1.9\t00/09/29\tMATRA UTIL"};



struct data_read
{
  int	datafd;			/* seule variable externe pour	*/
				/* d_close en lecture		*/
  long	pdat,pend;		/* pointeur sur donnees et fin	*/
  long	pcom[RES_COM_NUM];	/* pointeurs sur commentaires	*/
  long   pnom[RES_VAR_NUM];	/* pointeurs sur noms		*/
  int	vnum,cnum,rnum;		/* nombre de variables,comment	*/
				/*           enregistrements	*/
  int	recl;			/* Longueur d'un record		*/
  int	firstr; 		/* file descrip et flags	*/
  long	ll;
  int    use_dbl;               /* Utilisation des doubles */

};

typedef struct data_read data_read;

/* d_glob_rhandle cannot be static, because in the non reentrant version
of this lib, there's one single close function both for read and write,
and this function is in dataw.c*/
d_rhandle d_glob_rhandle = 0;

/*-----------------------------------------------------*\
|							|
|			d_ropen				|
|							|
\*-----------------------------------------------------*/

void	d_ropen (char *name)
{
  d_glob_rhandle = d_ropen_r (name,&_use_dbl);
  if(!d_glob_rhandle)
    {
      exit(1);
    }
    
}

d_rhandle	d_ropen_r (char *name, int* use_dbl)
{
  int	inw,n;
  char	c;
  char	did[6],nam[MAXPATHLEN];
  data_read* obj;


  obj = (data_read*)calloc(1, sizeof(data_read));
  if(!obj)
    {
      fprintf ( stderr, "libRES : *** Echec de l'allocation memoire\n");      
      return 0;
    }

  strcpy(nam, name);
  if(strcmp(nam+strlen(nam)-4, ".res") != 0)
    strcat(nam, ".res");

  obj->datafd = open (nam,(O_RDONLY));

  if(obj->datafd <=0 ) {
    fprintf ( stderr, "*** Erreur a l'ouverture de %s\n",nam);
    free(obj);
    return 0;
  }

	
  obj->pend  = lseek(obj->datafd,0L,2);	/*  pointeur sur le dernier byte	*/
  obj->ll    = lseek(obj->datafd,0L,0);	/*  position debut de fichier		*/

  read(obj->datafd,did,6);
  if((strcmp(did,"data2")!=0)&&(strcmp(did,"data3")!=0)) {
    obj->ll = lseek(obj->datafd,0L,0);
    obj->use_dbl = 0;
    obj->pcom[0] = 0;
  }
  else {
    if(strcmp(did,"data3")==0)
      obj->use_dbl = 1;
    else
      obj->use_dbl = 0;

    obj->pcom[0] = 6;
  }
  *use_dbl = obj->use_dbl;

  obj->cnum  = 0;
  obj->vnum  = 0;
  inw   = 0;			/* indicateur de zone en cours		*/
  obj->firstr= 1;			/* flag first read			*/


  while (inw != 2) {		/* Boucle de lecture jusqu'a la zone	*/
    /*  de donnees				*/

    n = read(obj->datafd,&c,1);	/* Lecture caractere			*/

    switch (c)	{
    case '\0' :		/* fin de chaine de caractere		*/
      switch (inw) {
      case 0 :	/* lecture commentaires			*/
	obj->pcom[++obj->cnum] = lseek(obj->datafd,0L,1);
	break;
      case 1 :	/* lecture noms				*/
	obj->pnom[++obj->vnum] = lseek(obj->datafd,0L,1);
	break;
      }
      break;
    case '\1' :		/* Fin de zone				*/
      switch (inw) {
      case 0 :	/* Fin de zone commentaires		*/
	inw++;
	obj->pnom[0] = lseek (obj->datafd,0L,1);
	break;
      case 1 :	/* Fin de zone noms			*/
	inw++;
	obj->pdat   = lseek (obj->datafd,0L,1);
	break;
      }
    }
  }

  obj->vnum = obj->vnum/2;

  if(obj->use_dbl)
    obj->recl = obj->vnum * sizeof(double);
  else
    obj->recl = obj->vnum * sizeof(float);

  obj->rnum = (int)(obj->pend-obj->pdat) /obj->recl;

  lseek (obj->datafd,obj->pdat,0);		/* Position debut de zone donnees	*/

  return obj;
}

/*-----------------------------------------------------*\
|							|
|			d_read				|
|							|
\*-----------------------------------------------------*/

int	d_read (void *buf)
{
  return d_read_r(d_glob_rhandle,buf); 
}

int	d_read_r (d_rhandle h, void *buf)
{
  data_read* obj = (data_read*)h;
  int	n;
  void  *p;

  if (obj->firstr) {
    lseek (obj->datafd,obj->pdat,0);
    obj->firstr = 0;
  }
  if ((n=read(obj->datafd,buf,obj->recl)) != obj->recl)
    return (EOF);

  /* FIXME : buf is supposed to be typed to float XOR double
     shall swap 32 or 64 according to my endianity */
  p=(void *)buf;
  if(obj->use_dbl)
    for(n=0; n<obj->recl; n+=sizeof(double))
      {
	DOUBLE_FROM_BE(p);
	p += sizeof(double);
      }
  else
    for(n=0; n<obj->recl; n+=sizeof(float))
      {
	FLOAT_FROM_BE(p);
	p += sizeof(float);
      }

  return (obj->recl);
}

int	d_restart_r (d_rhandle h)
{
  int ret=0;
  data_read* obj = (data_read*)h;
  ret = lseek (obj->datafd,obj->pdat,0);
  return ret;
}
    
/*-----------------------------------------------------*\
|							|
|			d_dread				|
|							|
\*-----------------------------------------------------*/
int	d_dread (void *buf, int i)
{
  return d_dread_r(d_glob_rhandle,buf,i);
}

int	d_dread_r (d_rhandle h, void *buf, int i)
{
  data_read* obj = (data_read*)h;
  int	n;
  long	l;

  l  = (long)(i * obj->recl) + obj->pdat;
  lseek (obj->datafd,l,0);

  if ((n=read(obj->datafd,buf,obj->recl)) != obj->recl)
    return (EOF);

  return (obj->recl);
}

/*-----------------------------------------------------*\
|							|
|			d_rcom				|
|							|
\*-----------------------------------------------------*/
void	d_rcom(char *com, int i)
{
  d_rcom_r(d_glob_rhandle, com,i);
    }

void	d_rcom_r(d_rhandle h, char *com, int i)
{
  data_read* obj = (data_read*)h;
  char	*p,
    cbuf[RES_COM_LEN];

  lseek (obj->datafd,obj->pcom[i],0);		/*  position sur le commentaire demande */
  read  (obj->datafd,cbuf,RES_COM_LEN);	/*  Lecture de RES_COM_LEN caracteres	*/

  p = cbuf;

  while ((*com++ = *p++) != '\0' )	/*  Recopie jusqu'au zero	*/
    ;
  *com++ = '\0';

  return;
} 

/*-----------------------------------------------------*\
|							|
|			d_rnam				|
|							|
\*-----------------------------------------------------*/
void	d_rnam(char *nam, char *desc, int i)
{
  d_rnam_r(d_glob_rhandle, nam, desc, i);
}

void	d_rnam_r(d_rhandle h, char *nam, char *desc, int i)
{
  data_read* obj = (data_read*)h;
  char	*p,
    cbuf[RES_NAME_LEN+RES_DESC_LEN];

  lseek (obj->datafd,obj->pnom[i*2],0);			/*  position sur la variable demandee */
  read  (obj->datafd,cbuf,RES_NAME_LEN+RES_DESC_LEN);	/*  Lecture de ??? caracteres	*/

  p = cbuf;

  while ( (*nam++ = *p++) != '\0' )		/*  Recopie nom jusqu'au zero	*/
    ;
  *nam++ = '\0';

  while ( (*desc++ = *p++) != '\0' )		/*  Recopie desc jusqu'au zero	*/
    ;
  *desc++ = '\0';

  return;
}

/*-----------------------------------------------------*\
|							|
|			d_rval				|
|							|
\*-----------------------------------------------------*/
int	d_rval(int c)
{
  return d_rval_r(d_glob_rhandle, c);
}

int	d_rval_r(d_rhandle h, int c)
{
  data_read* obj = (data_read*)h;
  switch  (c) {
  case 'v' :
    return(obj->vnum);
  case 'c' :
    return(obj->cnum);
  case 'r' :
    return(obj->rnum);
  }

  return(0);
}

void	d_rclos_r(d_rhandle h)
{
    data_read* obj = (data_read*)h;
    close(obj->datafd);
    free(obj);
    
}
