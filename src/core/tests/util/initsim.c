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
