/****************************************************************\
|*								*|
|*   Name	:						*|
|*								*|
|*	d_wopen		Open data file for write access		*|
|*	d_wcom		Write comment in data file		*|
|*	d_wnam		Ecriture du nom et descriptif		*|
|*			  d'une variable			*|
|*	d_writ		Ecriture d'un enregistrement		*|
|*	d_clos		Fermeture du fichier			*|
|*								*|
|*								*|
|*   Synopsis	:						*|
|*								*|
|*	d_wopen (name);						*|
|*	d_wcom  (com);						*|
|*	d_wnam  (nam,desc);					*|
|*	d_writ  (rec)						*|
|*	d_clos  ();						*|
|*								*|
|*								*|
|*	char	*name;		Nom du fichier			*|
|*	char	*com;		Ligne commentaire		*|
|*	char	*nam;		Nom de variable			*|
|*	char	*desc;		Descriptif de la variable	*|
|*	float	*rec;		Tableau de donnees		*|
|*								*|
|*   Description :  						*|
|*								*|
|*	Routines d'interface ecriture avec les fichiers de	*|
|*     	donnees							*|
|*								*|
|*								*|
\****************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <strings.h>

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

char	*dataw_sccsid={"@(#)dataw.c\t1.6\t93/06/22\tMATRA UTIL"};

int	datafd;				/* seule variable externe	*/
					/* pour d_clos	en lecture	*/
					/* pointeurs sur noms		*/
static	int	vnum,cnum,rnum;		/* nombre de variables,comment	*/
					/*           enregistrements	*/
static	int	recl;			/* Longueur d'un record		*/
static	int	firstw,firstn;	/* file descrip et flags	*/
/*-----------------------------------------------------*\
|							|
|			d_wopen				|
|							|
\*-----------------------------------------------------*/
void	md_wopen ( name)
char	*name;
{
char	nam[MAXPATHLEN],*pnam;

pnam = nam;
while ( *name != '.' && *name != '\0' && *name != ' ')
	*pnam++ = *name++;
*pnam++ = '.';
*pnam++ = 'r';
*pnam++ = 'e';
*pnam++ = 's';
*pnam++ = '\0';

#ifdef LAT
datafd = open (nam,(O_RAW | O_TRUNC | O_CREAT | O_RDWR),000600);
#else
datafd = open (nam,(O_TRUNC | O_CREAT | O_RDWR),
	(S_IWUSR | S_IWGRP | S_IRUSR | S_IRGRP));
#endif

if(datafd<=0) {
	fprintf ( stderr, "*** Erreur a l'ouverture de %s\n",nam);
	exit(1);
	}
firstw = 1;		/*  set first write flag	*/
firstn = 1;		/*  set first name  flag	*/
vnum   = 0;		/*  init nombre de var		*/

write (datafd,"data2",6);

}
/*-----------------------------------------------------*\
|							|
|			d_wcom				|
|							|
\*-----------------------------------------------------*/
void	md_wcom(com)
char	*com;
{
int	n;

n = write (datafd,com,strlen(com)+1);
}
/*-----------------------------------------------------*\
|							|
|			d_wnam				|
|							|
\*-----------------------------------------------------*/
void	md_wnam(nam,des)
char	*nam,*des;
{
int	n;

if (firstn) {				/* si premier appel ecriture	*/
	n  =  write (datafd,"\001",1);	/* pour delimiter comm et noms	*/
	firstn = 0;
	}
n = write (datafd,nam,strlen(nam)+1);
n = write (datafd,des,strlen(des)+1);
vnum++;					/* compeur du nombre de var	*/
}
/*-----------------------------------------------------*\
|							|
|			d_writ				|
|							|
\*-----------------------------------------------------*/
void	md_writ(buf)
char	*buf;
{
int	n;

#ifdef MC
int	k;
char	ct,*pbuf;
#endif

if (firstw) {				/* si premier write ecriture	*/
	n = write (datafd,"\001",1);	/* de \001 pour delimiter	*/
	firstw = 0;			/* les zones noms et data	*/
	recl = vnum * sizeof(float);	/* nombre de char par record	*/
	}

#ifdef MC
pbuf = buf;
for (k=0;k<vnum;k++) {
	ct = *pbuf;
	*pbuf = *(pbuf+3);
	*(pbuf+3) = ct;
	ct = *(pbuf+1);
	*(pbuf+1) = *(pbuf+2);
	*(pbuf+2) = ct;
	pbuf += 4;
	}
#endif

if ((n = write (datafd,buf,recl)) != recl) {
	fprintf (stderr,"*** d_writ : Erreur a l'ecriture\n");
	exit(1);
	}

}
/*-----------------------------------------------------*\
|							|
|			d_clos				|
|							|
\*-----------------------------------------------------*/
void	md_clos()
{
close(datafd);
}

