/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/nl.c,v 1.2 2002-12-18 16:27:48 tntdev Exp $

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
|*   Name	:  nl.c						*|
|*								*|
|*	opnl		Ouverture namelist			*|
|*	adnl		Declaration d'un objet			*|
|*	rdnl		Lecture de la namelist			*|
|*								*|
|*   Synopsis	:						*|
|*								*|
|*	opnl ( name);						*|
|*	adnl ( name, ad, f);					*|
|*	rdnl ();						*|
|*								*|
|*   Description : 						*|
|*	Routine en c de "simulation" de format namelist		*|
|*	 de la dynamique dycemo					*|
|*								*|
\****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "unit.h"

char	*nl_sccsid={"@(#)nl.c\t1.11\t00/09/29\tMATRA UTIL"};

#define LTOK 1000
#define LUNIT 80
#define MAXN 500
static	char	fo[MAXN];
static	char	*pnaml[MAXN];
static	int	lnaml[MAXN];
static	char	*adr[MAXN];
static  char	nlunit[LUNIT];
static	char	*strucn;
static	int	nv;
static	char	*cad,*lad;
static	int	ncom = 0;
static	char 	typen;
static  FILE	*stdi = 0;
 
#define NUM	1
#define FIN	0
#define DEC	2
#define STRING	3
#define NAML	4

/*---------------------------------------------*\
|						|
|	opnl (entree C)				|
|						|
\*---------------------------------------------*/
void	opnl ( name)
char	*name;
{

char	tok[400];

strucn = name;
if (next(tok) != NAML) {
	fprintf ( stderr, "%s  is not a namelist",tok);
	exit(1);
	}
if (strncmp(tok+1,name,strlen(tok+1)) != 0) {
	fprintf ( stderr, "program expect %s and not %s\n",name,tok);
	exit(1);
	}
nv	=  0;
}

/*---------------------------------------------*\
|						|
|	opnl (entree FORTRAN)			|
|						|
\*---------------------------------------------*/
void	opnl_ ( name, lnam)
char	*name;
long	lnam;
{
char	ccop[50];
strncpy ( ccop, name, (int)lnam);
ccop[(int)lnam] = '\0';
opnl ( ccop);
}

/*---------------------------------------------*\
|						|
|	adnl (entree C)				|
|						|
\*---------------------------------------------*/
void	adnl ( nama, ada, foa)
char	*nama;
char	*foa;
char	*ada;
{
pnaml[nv]	=  nama;
lnaml[nv]	=  strlen(pnaml[nv]);
fo[nv]		=  *foa;
adr[nv++]	=  ada;

}

/*---------------------------------------------*\
|						|
|	adnl (entree FORTRAN)			|
|						|
\*---------------------------------------------*/
void	adnl_( nama, ada, foa, lnama)
char	*nama;
char	*foa;
char	*ada;
long	lnama;
{
pnaml[nv]	=  nama;
lnaml[nv]	=  (int)lnama;
fo[nv]		=  *foa;
adr[nv++]	=  ada;

}

/*---------------------------------------------*\
|						|
|	rdnl (entree C)				|
|						|
\*---------------------------------------------*/
void	rdnl ()
{
int	c,i,convert;
char	tok[LTOK];
char	*poi;
double	fac,wlec;
struct sunit res;


while ((c= next (tok)) != NAML ) {		/************************/
						/* boucle tant que pas	*/
						/* &xxx			*/
	if (c == DEC){				/*----------------------*/
						/* declaration          */

		res.conv = 1.;
		i = trait (tok);
		if (*nlunit != '\0')
			res = si_unit (nlunit);
		}

	else if (c == FIN) {
		fprintf (stderr,"*** nl: end of file during namelist read\n");
		exit(1);
		}
	else {
		poi = tok;				/* recherche d'une */
		while (*poi != '\0' && *poi != '/')	/* zone /deg       */
			poi++;				/* indiquant une   */
		if (*poi == '/'){			/* convertion	   */
			convert = 1;			/* poi pointe sur  */
			}
		else {
			convert = 0;
			}
		*poi++ = '\0';				/* deg		   */
					
		if (cad >= lad) {
			fprintf (stderr,
			"***  nl: Trop de donnees Structure %s Donnees %s\n",
			strucn,pnaml[i]);
			exit(1);
			}

		fac = res.conv;
		if (convert) {
			if (strncmp (poi,"deg",3) == 0) {
				fac =  0.01745329251994329576;
				}
			}

		switch (fo[i]) {
		case 's' :
			if (sscanf (tok,"%hd",cad) != 1) {
				fprintf ( stderr,
				"illegal token in namelist : %s\n",tok);
				exit(1);
				}
			cad += sizeof (short);
		break;
		case 'l' :
			if (sscanf (tok,"%ld",cad) != 1) {
				fprintf ( stderr,
				"illegal token in namelist : %s\n",tok);
				exit(1);
				}
			cad += sizeof (long);
		break;
		case 'i' :
			if (sscanf (tok,"%d",cad) != 1) {
				fprintf ( stderr,
				"illegal token in namelist : %s\n",tok);
				exit(1);
				}
			cad += sizeof (int);
		break;
		case 'd' :
			if (sscanf (tok,"%lf",&wlec) != 1) {
				fprintf ( stderr,
				"illegal token in namelist : %s\n",tok);
				exit(1);
				}
			*((double *)cad) = (double) (fac * wlec);
			cad += sizeof (double);
		break;
		case 'f' :
			if (sscanf (tok,"%lf",&wlec) != 1) {
				fprintf ( stderr,
				"illegal token in namelist : %s\n",tok);
				exit(1);
				}
			*((float *)cad) = (float) (fac * wlec);
			cad += sizeof (float);
		break;
		case 'c' :
		   if (typen=='f') {
			strncpy (cad,tok,strlen(tok));
			cad +=  strlen(tok);
			}
		   else {
			strcpy (cad,tok);
			cad +=  strlen(tok);
			}
		break;
		default :
			fprintf ( stderr, "*** nl: type %c illegal\n",c);
		break;
			}
		}
	}
if (strcmp(tok,"&end") != 0) {
	fprintf ( stderr, "program expect &end and not %s\n",tok);
	exit(1);
	}
}

/*---------------------------------------------*\
|						|
|	rdnl (entree FORTRAN)			|
|						|
\*---------------------------------------------*/
rdnl_()
{
rdnl();
}
int	next(tk)
char	*tk;
{
int	c,i;

while ( ((c=getnc())==' ')||(c=='\t')||(c==',')||(c=='\n')) 
	;

if (isalpha(c) != 0 ) {
	*tk++ = c;
	while(((c=getnc())!='=') && (c != EOF)) 
			*tk++ = c;
	i = DEC;
	}
else if ( c == '\"' ) {

	while(((c=getnc())!='\"') && (c != EOF)) {
		if (c == '\\') 
			c = getnc();
		*tk++ = c;
		}
	i = STRING;
	}
else if ( c == '&') {
	*tk++ = c;
	while(!(((c=getnc())==' ')||(c=='\t')||(c==',')||(c=='\n')||(c==EOF))) 
		*tk++ = c;
	i = NAML;
	}
else {
	* tk++ = c;
	while(!(((c=getnc())==' ') || (c=='\t')||(c==',')||(c=='\n')||(c==EOF))) 
		*tk++ = c;
	i = NUM;
	}
if ( c == EOF )
	i = FIN;
*tk++ = '\0';
return (i);
}	
		
trait (tkn)
char	*tkn;
{
int	l,nn,ni,nt,k,nc,j,s;
int	i[3],n[3],ifo[3],nfo[3];
char	nac[40],nal[40],cop[100];
char	*poi,*pcop,typei,*p1,*p2,*next,*dest;

#define ISALNUM(c) (('a'<=(c)&&(c)<='z') || ('A'<=(c)&&(c)<='Z') || ('0'<=(c) && (c)<='9') || (c)=='_')

for (l=0;l<nv;l++) {

/* test rapide */

	p1 = tkn;
	p2 = pnaml[l];

	while( *p1 == *p2 && ISALNUM(*p1))  {
		p1++; p2++;
		}
/* si un des 2 caracteres est alphanum alors noms differents */

	if( ISALNUM(*p1) || ISALNUM(*p2))
		continue;
/* test complet */
	
	poi = tkn;
	pcop = cop;
	while (*poi != '\0' && *poi != '{') {
		if (*poi == '[' || *poi == ']') {
			typei = 'c';
			*pcop++ = ' ';
			}
		else if (*poi == '(' || *poi == ')' || *poi == ','){
			typei = 'f';
			*pcop++ = ' ';
			}
		else {
			*pcop++ = *poi;
			}
		poi++;
		}
	*pcop++ = '\0';

/* isolation du champ unite si present */
/*-------------------------------------*/
 
	pcop = nlunit;
	if(*poi++ == '{') {
		while (*poi != '\0' && *poi != '}' && pcop <= &nlunit[LUNIT-2])
			*pcop++ = *poi++;
		}
	*pcop++ = '\0';
#ifdef i860
	ni = 1;
	pcop = cop;
	dest = nac;
	while(*pcop!=' ' && *pcop != '\t' && *pcop != '\0') {
		*dest++ = *pcop++;
		}
	*dest = '\0';
	i[0] = (int)strtol(pcop,&next,10);
	if(next==pcop) {
		goto end;
		}
	ni++; pcop = next++;
	i[1] = (int)strtol(pcop,&next,10);
	if(next==pcop) {
		goto end;
		}
	ni++; pcop = next++;
	i[2] = (int)strtol(pcop,&next,10);
	if(next==pcop) {
		goto end;
		}
	ni++; pcop = next++;
end:	
		
#else

	ni = sscanf (cop,"%s %d %d %d",nac,&i[0],&i[1],&i[2]);
#endif
	for (j=0,poi=pnaml[l];j<lnaml[l];j++,poi++){
		if (*poi == '[' || *poi == ']') {
			typen = 'c';
			cop[j] = ' ';
			}
		else if (*poi == '(' || *poi == ')' || *poi == ','){
			typen = 'f';
			cop[j] = ' ';
			}
		else {
			cop[j] = *poi;
			}
		}
	cop[j] = '\0';
#ifdef i860
	nn = 1;
	pcop = cop;
	dest = nal;
	while(*pcop!=' ' && *pcop != '\t' && *pcop != '\0') {
		*dest++ = *pcop++;
		}
	*dest = '\0';
	n[0] = (int)strtol(pcop,&next,10);
	if(next==pcop) {
		goto end1;
		}
	nn++; pcop = next++;
	n[1] = (int)strtol(pcop,&next,10);
	if(next==pcop) {
		goto end1;
		}
	nn++; pcop = next++;
	n[2] = (int)strtol(pcop,&next,10);
	if(next==pcop) {
		goto end1;
		}
	nn++; pcop = next++;
end1:	
		
#else
	nn = sscanf (cop,"%s %d %d %d",nal,&n[0],&n[1],&n[2]);
#endif
	if (strcmp(nac,nal) != 0)
		continue;

	nn--;
	ni--;

	/* pas de dimension dans la definition  */
	/* mise a 1 par defaut			*/
	/*--------------------------------------*/
	if (nn == 0) {
		nn = 1;
		n[0] = 1;
		}

	/* pas de dimension dans la donneei	*/
	/* mise a 1 par defaut type c		*/
	/*--------------------------------------*/
	if (ni == 0) {
		ni = 1;
		i[0] = 0;
		typei = 'c';
		}
	if (ni != nn && typei == 'f' ) {
		fprintf (stderr,"*** nl : erreur de dimension de %s\n",tkn);
		exit(1);
		}
	if (ni > nn)
		ni = nn;

	for (j=0,nt=1;j<nn;j++)
		nt *= n[j];

	for (j=ni;j<nn;j++)
		i[j]=0;

	/* transformation en tableau type c */
	/*----------------------------------*/
	for (j=0;j<nn;j++){
		if (typen == 'f')
			nfo[j] = n[nn-1-j];
		else
			nfo[j] = n[j];
		if (typei == 'f')
			ifo[j] = i[nn-1-j]-1;
		else
			ifo[j] = i[j];
		}


	/* calcul du nombre total d'elements nc */
	/*--------------------------------------*/
	for (j=0,nc=0;j<nn;j++) {
		for (k=j+1,s=1;k<nn;k++)
			s *= nfo[k];
		nc += ifo[j] * s;
		}

	switch (fo[l]) {
	case 's' :
		cad = adr[l] + nc * sizeof (short);
		lad = adr[l] + nt * sizeof (short);
		break;
	case 'l' :
		cad = adr[l] + nc * sizeof (long);
		lad = adr[l] + nt * sizeof (long);
		break;
	case 'i' :
		cad = adr[l] + nc * sizeof (int);
		lad = adr[l] + nt * sizeof (int);
		break;
	case 'f' :
		cad = adr[l] + nc * sizeof (float);
		lad = adr[l] + nt * sizeof (float);
		break;
	case 'd' :
		cad = adr[l] + nc * sizeof (double);
		lad = adr[l] + nt * sizeof (double);
		break;
	case 'c' :
		cad = adr[l] + nc ;
		lad = adr[l] + nt ;
		poi = cad;
		if (typen == 'f') {
			*poi = ' ';
			while (++poi < lad) {
				*poi = ' ';
				}
			}
		break;
	default :
		fprintf ( stderr, "*** nl: type %c illegal\n",fo[l]);
	break;
		}
	return l;
	}
fprintf (stderr,"*** nl : nom de variable %s non declare\n",tkn);
exit(1);
}

int	getnc(void)
{
int	c,c1;

 if( 0 == stdi)
   stdi = stdin;

loop :
c = getc(stdi);
if ( c == EOF ) return (c);
if ( c == '/' ) {
	c1 = getc (stdi);
	if ( c1 == '*') {
		ncom++;
		}
	else 
		ungetc (c1,stdi);
	}
else if ( c == '*') {
	c1 = getc (stdi);
	if ( c1 == '/') {
		ncom--;
		goto loop;
		}
	else 
		ungetc (c1,stdi);
	}

if ( ncom != 0 ) 
	goto loop;
return c;
}

void nlsetfile (char *name)
{
  if( 0 == stdi)
    stdi = stdin;
 

	if(stdi != stdin)
		fclose(stdi);

	if ( (stdi = fopen(name,"r")) == NULL ) {
		fprintf(stderr,"*** nl : unable to open %s\n",name);
	}
}

void nexts (char *tk)
{
	int	c;

	while ((c=getnc()) == ' ' || (c == '\t') || (c=='\n'))
		;

	*tk++ = c;

	while ((c=getnc()) != ' ' && (c != '\t') && (c!='\n') && (c!=EOF))
		*tk++ = c;

	*tk++ = '\0';
}

