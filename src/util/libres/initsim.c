/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/initsim.c,v 1.1 2003-01-31 18:32:56 tsp_admin Exp $

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
|*		   initsim	Initialisation d'une simulation *|
|*   Synops	:						*|
|*		   initsim ( )					*|
|*								*|
|*   Fonction:  Ouverture du fichier de resultats		*|
|*      	et ecriture du username,de la date,du nom	*|
|*		du fichier de donnees et des titres		*|
|*		lus dans le fichier de donnees			*|
|*								*|
\****************************************************************/
extern	char	**environ[];
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

void	adnl(),rdnl(),opnl();
void	d_wopen(),d_wcom();

char	*initsim_sccsid={"@(#)initsim.c\t1.6\t00/09/29\tMATRA UTIL"};
void	initsim_()
{
int	fail,i,j,k;
char	titre[400],stitre[400],**envp;

envp = *environ;


fail = 1;
for (i=0;envp[i] != NULL;i++) {
	if (strncmp(envp[i],"DATA=",5) == 0 ) {
		fail = 0;
		strcpy ( titre, envp[i]+5);
		for (j=1,k=0;j<200;j++) {
			if (titre[j]== '.' && titre[j+1] == 'd'){
				titre[j] = '\0';
				break;
				}
			else if (titre[j] == '\0' )
				break;
			else if ( j== 199) {
				exit(1);
			}
			if ( titre[j]=='/' )
				k = j+1;
		}
		d_wopen (&titre[k]);
		}
	}
if (fail) 
	d_wopen("prog");


fail = 1;
for (i=0;envp[i] != NULL;i++) {
	if (strncmp(envp[i],"DATE=",5) == 0) {
		fail = 0;
		d_wcom (envp[i]+5);
		}
	}
if (fail)
	d_wcom("date");

fail = 1;
for (i=0;envp[i] != NULL;i++) {
	if (strncmp(envp[i],"USER=",5) == 0) {
		fail = 0;
		d_wcom (envp[i]+5);
		}
	}
if (fail)
	d_wcom("user");


fail = 1;
for (i=0;envp[i] != NULL;i++) {
	if (strncmp(envp[i],"DATA=",5) == 0) {
		fail = 0;
		d_wcom (envp[i]+5);
		}
	}
if (fail)
	d_wcom("data");

fail = 1;
for (i=0;envp[i] != NULL;i++) {
	if (strncmp(envp[i],"PROG=",5) == 0) {
		fail = 0;
		d_wcom (envp[i]+5);
		}
	}
if (fail) 
	d_wcom ("prog");

opnl ("titre");
adnl ("titre[400]"	,titre	,"c");
adnl ("stitre[400]"	,stitre	,"c");
rdnl();

d_wcom (titre);
d_wcom  (stitre);
}
