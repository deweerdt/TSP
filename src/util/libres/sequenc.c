/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/sequenc.c,v 1.1 2003-01-31 18:32:56 tsp_admin Exp $

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

void	adnl(),rdnl(),opnl();

#define NMODX 40
char	*sequenc_sccsid={"@(#)sequenc.c\t1.8\t00/09/29\tMATRA UTIL"};
extern	double	t_;
static	double	tmax,deltat;
static	double	nb_pas,tranche,pourcentage,coeff;
static	int	compteur_pas;
static	long	count[NMODX][2];
static	nmod=0;
int	nsym=0;
char	*(*symadr[100])();
char	symnam[NMODX][20];

static	char	modnam[NMODX][20];
static	void	(*modadr[100])();

seq_sym ( name, adr)
char	*name;
char	*(*adr)();
{
if (strlen(name)>16) {
	fprintf(stderr,"*** seq_sym : nom de fonction %s trop long\n",name);
	exit(1);
	}
strcpy (&symnam[nsym][0],name);
symadr[nsym] = adr;
nsym++;
}
seq_sym_ ( name, adr, lname)
char	*name;
char	*(*adr)();
long	lname;
{
strncpy (&symnam[nsym][0],name,lname);
symnam[nsym][lname] = '\0';
symadr[nsym] = adr;
nsym++;
}

seq_mod ( name, adr)
char	*name;
void	(*adr)();
{
if (strlen(name)>16) {
	fprintf(stderr,"*** seq_mod : nom de fonction %s trop long\n",name);
	exit(1);
	}
strcpy (&modnam[nmod][0],name);
modadr[nmod] = adr;
nmod++;
}
seq_mod_ ( name, adr, lname)
char	*name;
void	(*adr)();
long	lname;
{
strncpy (&modnam[nmod][0],name,lname);
modnam[nmod][lname] = '\0';
modadr[nmod] = adr;
nmod++;
}
sequenceur_i_()
{
int	i,j;

for (i=0;i<NMODX;i++) {
	count[i][0] = 0;
	count[i][1] = 0;
	}

tmax = 0;
deltat = 0.;

opnl("sequenceur");
adnl ("tmax",	&tmax,	"d");
adnl ("deltat",	&deltat,	"d");
for (i=0;i<nmod;i++) 
	{
	strcat ( &modnam[i][0], "(2)");
	adnl (&modnam[i][0],	&count[i][0],	"l");
	}
rdnl();

for (i=0;i<nmod;i++) {
	for (j=0;j<16;j++) {
		if(modnam[i][j]=='(')
			modnam[i][j] = '\0';
		}
	}
for (i=0;i<nmod;i++)
	{
	count[i][1] = count[i][0] - count[i][1];
	}

/*******************************************************/
/* init de l'algo de calcul de l'avancement de la simu */
/*******************************************************/
/* raz des variables */
compteur_pas = -1; /* pour ne pas compter le premier pas durant l'init */
pourcentage = 0;

/* calcul du nombre de pas de la simu */
nb_pas = tmax / deltat;

/* calcul du nombre de pas correspondant a 10% de la simu */
tranche = 0.1 * nb_pas;

/* si il y a moins de 10 points, il est impossible de */
/* travailler avec une resolution de 10%, il faut plus */
if (nb_pas < 10)
      {
      coeff = 10.0 / nb_pas;
      }
else
      {
      coeff = 1.0;
      }
}



sequenceur_()
{
int	i;
do
	{
	for (i=0;i<nmod;i++)
	  {
	  if (count[i][0] == count[i][1] && count[i][0] != 0) {
		count[i][1] = 0;
		(*modadr[i])();
		}
	  }
	for (i=0;i<nmod;i++)
		count[i][1]++;

      /*************************************/
      /* calcul de l'avancement de la simu */
      /*************************************/
      /* compteur sur le nombre de pas de simu */
      compteur_pas++;

      if (compteur_pas >= tranche)
      {
              compteur_pas -= tranche;
              compteur_pas += 0.01; /* contre les erreurs d'arrondi intempestif */
              pourcentage += 10 * coeff;

              if ( (nb_pas < 10)&&(pourcentage > 90) )
                      pourcentage = 100;  /* pour les arrondis finaux */

              /* affichage de l'avancement en % */
              fprintf ( stderr,"%d%% completed\n",(int) pourcentage);
      }

} while (t_ <= tmax);
}



double	cstep (name)
char	*name;
{
int	i;

for (i=0;i<nmod;i++) {
	if (strcmp (&modnam[i][0],name)==0)
		return(deltat*(double)count[i][0]);
	}
fprintf ( stderr,"*** cstep : module %s non declare\n",name);
exit(1);
}

double	cstep_ (name,lname)
char	*name;
long	lname;
{
char	buf[50];

strncpy ( buf, name, (int)lname);
buf[lname] = '\0';
return (cstep(buf));
}
stopsim()
{
tmax = 0.;
}
