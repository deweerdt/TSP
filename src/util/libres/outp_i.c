/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/outp_i.c,v 1.1 2003-01-31 18:32:56 tsp_admin Exp $

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
|*   Name	:						*|
|*								*|
|*	outp_i_		Initialisation et lecture des donnees	*|
|*	outp_		Force l'ecriture des resultats		*|
|*								*|
|*   Synopsis	:						*|
|*								*|
|*	outp_i_	();						*|
|*	outp_();						*|
|*								*|
|*								*|
|*   Description :						*|
|*								*|
|*	Routines de sortie des resultats			*|
|*								*|
|*	Les interfaces exterieures sont :			*|
|*								*|
|*	  * Le fichier de donnees stdin				*|
|*	  * Le fichier de symboles permettant d'associer	*|
|*	     a un nom symbolique l'adresse corespondante	*|
|*								*|
|*								*|
|*								*|
\****************************************************************/
extern	char	**environ[];
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/param.h>

#include  "libUTIL.h"
#include  "unit.h"

char	*outp_i_sccsid={"@(#)outp_i.c\t1.10\t00/09/29\tMATRA UTIL"};

#define	LISTLEN	4500
#define	NVMAX	300
#define TOKLEN	200

struct symbole {
	char	*addr;
	char	typ;
	char	*units;
	};

extern	int	nsym;
extern	char	*((*symadr[100])());
extern	char	symnam[40][20];
extern	char	*tokex();
static	char	unit[80];

struct {
	int	nv;
	char	*v_adr[NVMAX];
	char	v_typ[NVMAX];
	double	v_conv[NVMAX];
	FILE	*symfp;
	} outp;

struct variable	{
	char	*addr;
	char	typ;
	double	conv;
	char	*name;
	char	*units;
	};

void	outp_i_(void)
{

int	i,k,c,errflg;
int	fail;
char	**envp;
char	list[LISTLEN],tok[110],tok1[110],sym[30],desc[TOKLEN],sta[30];
char	*plist,*plistn,*pnam[NVMAX];
int	voff[NVMAX];
long	fpos[NVMAX];
char	symfile[MAXPATHLEN],*tmp,tmpfile[MAXPATHLEN],*pc,*p;

struct	variable var,variable();
FILE	*stm;

int	getsym();
char	*ncmp();
char	*caddr();
int	off;
double	conv;
char	typ;

/*---------------------------------------------*\
|	Lecture du fichier d'entree		|
|	sur l'unite stdin (fd=0)		|
|	le format est trivial :			|
|						|
|	&outp					|
|	nom1 nom2 nom3				|
|	&end					|
|						|
\*---------------------------------------------*/
nexts(tok);
if  (strcmp(tok,"&outp")!= 0) { 	 		   /* verif	*/
	fprintf (stderr,"*** outp_i : expect &outp and not %s\n",tok); /* &outp	*/
	exit(1);					    /*		*/
	}


plist = list;				/* Init pointeur sur liste des	*/
outp.nv  = 0;				/*  noms et nombre de var	*/
for (;;) {
	nexts(tok);
	if (*tok == '&')
		break;
	pnam[outp.nv++] = plist;	/* Memorisation de la position	*/
	plistn = plist +strlen(tok)+1;
	if (plist > &list[LISTLEN]) {
		fprintf(stderr,"*** outp_i : Trop de nom de variables\n");
		exit(1);
		}
	strcpy ( plist, tok);		/*  du nom nv dans la list	*/
	plist = plistn;
	}

if (strcmp(tok,"&end") != 0) {		/* Verif &end			*/
	fprintf (stderr,"*** outp_i : expect &end and not %s\n",tok);
	exit(1);
	}
if (outp.nv > NVMAX) {
	fprintf(stderr,"*** outp_i : Trop de variables en sortie\n");
	exit(1);
	}

envp = *environ;

fail = 1;
for (i=0;envp[i] != NULL;i++) {
	if (strncmp(envp[i],"DATA=",5) == 0) {
		fail = 0;
		strcpy (tmpfile,envp[i]+5);
		}
	}
if (fail) 
	strcpy (tmpfile,"data.d");

pc = tmpfile;
i = strlen (tmpfile);
if ( strcmp(&tmpfile[i-2],".d") != 0) {
	fprintf (stderr,"*** outp_i : le nom du fichier de donnees doit etre termine par .d\n");
	exit(1);
	}
strcpy ( &tmpfile[i-2], ".tmp");
for (i=strlen(tmpfile);i>=0;i--)
	if(tmpfile[i]=='/')
		break;
tmp = &tmpfile[i+1];

/* ouverture du fichier symbole */
/*------------------------------*/
envp = *environ;

fail = 1;
for (i=0;envp[i] != NULL;i++) {
	if (strncmp(envp[i],"PROG=",5) == 0) {
		fail = 0;
		strcpy (symfile,envp[i]+5);
		}
	}
if (fail) 
	strcpy (symfile,"prog");
pc = symfile;

strcat (symfile,".sym");

if((outp.symfp = fopen (symfile,"r")) == NULL) {
	fprintf ( stderr,"*** outp_i : Erreur a l'ouverture de %s\n",symfile);
	exit(1);
	}

/* recherche des variables dans le catalogue */
/*********************************************/

if((stm = fopen (tmp,"w")) == NULL) {
	fprintf ( stderr,"*** outp_i : Erreur a l'ouverture de %s\n",tmp);
	exit(1);
	}

for(i=0;i<outp.nv;i++) {
	var = variable (pnam[i]);
	if (var.addr != NULL) {
		outp.v_adr[i] = var.addr;
		outp.v_conv[i] = var.conv;
		outp.v_typ[i] = var.typ;
		fpos[i] = ftell( stm);
		p = var.name;
		while (*p != '\0')
			fputc( *p++, stm);
		fputc ( '\0', stm);	
		fputc ( '{', stm);	
		p = var.units;
		while (*p != '\0')
			fputc( *p++, stm);
		fputc ( '}', stm);	
		fputc ( '\0', stm);	
		
		pnam[i] = NULL;
		}
	}

while (getsym(sym,desc,tok1) != EOF ) {
	for (i=0;i<outp.nv;i++) {	
		if ((pnam[i] != NULL)&&((ncmp(pnam[i],sym)) != NULL)){
			strcpy ( tok,tok1);
			addoff (tok,pnam[i]);
			var = variable( tok);
			outp.v_adr[i] = var.addr;
			outp.v_conv[i] = var.conv;		
			outp.v_typ[i] = var.typ;
			strcat ( desc, " {");
			strcat ( desc, var.units);
			strcat ( desc, "}");
			fpos[i] = ftell( stm);
			p = pnam[i];
			while (*p != '\0')
				fputc( *p++, stm);
			fputc ( '\0', stm);	
			p = desc;
			while (*p != '\0')
				fputc( *p++, stm);
			fputc ( '\0', stm);	
			if (var.addr != NULL)
				pnam[i] = NULL;
			}
		}
	}
fclose (outp.symfp);
fclose (stm);
if((stm = fopen (tmp,"r")) == NULL) {
	fprintf ( stderr,"*** outp_i : Erreur a l'ouverture de %s\n",tmp);
	exit(1);
	}

/* verification du fait que tous les noms ont ete trouves */
/*--------------------------------------------------------*/

errflg = 0;
for (i=0;i<outp.nv;i++) {
	if (pnam[i] != NULL) {
		fprintf (stderr,"*** outp_i : variable %s non definie\n",
		pnam[i]);
		errflg++;
		}
	}
if (errflg)
	exit(1);

/* recopie des noms et descripteurs dans le fichier resultat */
/*-----------------------------------------------------------*/

for (i=0;i<outp.nv;i++) {
	fseek( stm, fpos[i], 0);
	p = tok;
	while ( (c = fgetc(stm)) != '\0' && c != EOF)
		*p++ = c;
	*p++ = '\0';
	p = desc;
	while ( (c = fgetc(stm)) != '\0' && c != EOF)
		*p++ = c;
	*p++ = '\0';
	if (c == EOF) {
		fprintf (stderr,"*** outp_i : errur de lecture dans %s\n",tmp);
		exit(1);
		}
	d_wnam ( tok, desc);
	}
fclose (stm);
unlink (tmp);
}
struct	variable	variable (name)
char *name;
{
struct	variable res;
struct	sunit ounit, nunit;
char	module[16],typ;
int	off,i;
char	*pu,*p,*ps;

res.units = res.name = "\0";
res.typ = 'u';
p = tokex ( name, module, &off);
if (p == NULL) {
	res.addr = NULL;
	return(res);
	}

for (i=0;i<nsym;i++) {
	if(strcmp(&symnam[i][0],module)==0)
		break;
	}
if (i == nsym){
	res.addr = NULL;
	return (res);
	}
while (*p!='{' && *p != '\0') 
	p++;
if ( *p == '{' ) {
	*p++ = '\0';	/* p pointe sur l'unite demandee */
	ps = p;
	while (*ps != '}' && *ps != '\0') 
		ps++;
	if (*ps == '\0') {
		fprintf (stderr,"*** variable : erreur de syntaxe dans (%s)\n",name);
		exit(1);
		}
	*ps = '\0';
	}
unit[0] = '\0';
res.addr = (*symadr[i])(name,&typ,unit,(long)strlen(name),1L,80L);

/* mise d'un \0 apres le dernier caractere non blanc */
/*---------------------------------------------------*/

pu = &unit[79];
while (*pu == ' ' || *pu =='\0')
	pu--;
*++pu = '\0';
res.name = name;
res.typ	= typ;
res.units = unit;
if (*p != '\0'){
	res.units = p;
	nunit = si_unit (p);
	ounit = si_unit (unit);
	if (cmp_unit(&nunit.d, &ounit.d) != 0) {
		fprintf(stderr,"*** outp : erreur conversion de %s : {%s} -> {%s}\n",name,unit,p);
		exit(1);
		}
	else {
		res.conv = ounit.conv/nunit.conv;
		}
	}
else {
	res.conv = 1.;
	}
return (res);
}
addoff (tok,ntok)
char *tok,*ntok;
{
char	*unit1,*unit2,*p,*nscan();
char	buf[100];
int	off1,off2;

if ((p = nscan(tok,&off1,&unit1)) == NULL) {
	fprintf (stderr,"*** outp_i : erreur de syntaxe dans %s\n",tok);
	exit(1);
	}
if ((nscan(ntok,&off2,&unit2)) == NULL) {
	fprintf (stderr,"*** outp_i : erreur de syntaxe dans %s\n",ntok);
	exit(1);
	}
if (*unit2 != '\0') {
	strcpy ( buf, unit2);
	}
else {
	strcpy ( buf, unit1);
	}
sprintf ( p+1,"[%d]{",off1+off2);
strcat (tok,buf);
strcat (tok,"}");
}

char	*ncmp ( nam, sym)
char	*nam,*sym;
{
while (*nam == *sym && *nam != '\0') {
	nam++;
	sym++;
	}
if ( (*nam == '(' || *nam == '[' || *nam == '{' || *nam == '\0' ) && *sym == '\0')
	return(nam);
else
	return(NULL);
}

