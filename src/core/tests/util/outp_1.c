#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include "libUTIL.h"

char	*outp_1_sccsid={"@(#)outp_1.c\t1.6\t00/09/29\tMATRA UTIL"};


extern	int	nsym;
extern	char	*(*symadr[100])();
extern	char	symnam[40][20];
extern	char	*tokex();

#define	LISTLEN	4500
#define	NVMAX	300
#define TOKLEN	200

extern	struct {
	int	nv;
	char	*v_adr[NVMAX];
	char	v_typ[NVMAX];
	double	v_conv[NVMAX];
	FILE	*symfp;
} outp;



int	getsym(char *sym, char *desc, char *def)
{
	char	tok[TOKLEN];
	int	n;

	/*---------------------------------------------*\
	|						|
	|	Lecture du fichier symboles		|
	|	dont les enregistrements		|
	|	doivent etre de la forme :		|
	|						|
	|	sym \t common \t typ \t conv \n		|
	|		descripteur			|
	|						|
	|	Les separateurs sont \t et \n		|
	|	les blancs sont donc significatifs	|
	|						|
	\*---------------------------------------------*/
	if ((n=gnext(tok)) == EOF )
		return(EOF);	/* Fin normale	*/

	if (n >= 16) {
		fprintf (stderr,
			"*** outp_i : Symbole trop long dans le fichier symboles\n");
		exit(1);
	}

	strcpy (sym,tok);

	if (gnext(tok) == EOF ) {
		fprintf (stderr,"*** outp_i : fin prematuree du fichier symboles");
		exit(1);
	}

	strcpy (def,tok);

	if ((n=gnext(tok)) == EOF ) {
		fprintf (stderr,"*** outp_i : fin prematuree du fichier symboles");
		exit(1);
	}
	if ( n >= TOKLEN ) {
		fprintf (stderr,"*** outp_i : Descriptif trop long\n");
		exit(1);
	}

	strcpy (desc,tok);

	return (1);
}

int	gnext(char *tok)
{
	int	c,i;
	char	*sav;

	sav = tok;

	while (((c=getc(outp.symfp))=='\t') || (c=='\n'))
		;

	if ( c== EOF)
		return (EOF);

	i = 0;
	*tok++ = (char)c;
	while (((c=getc(outp.symfp))!='\t') && (c!='\n') && (c!=EOF)) {
		if ( i++ >= TOKLEN ) {
			fprintf ( stderr,
				"*** outp_i : chaine trop longue dans symboles\n");
			exit(1);
		}
		*tok++ = (char)c;
	}

	if(c==EOF)
		return (EOF);

	*tok++ = '\0';

	return(i);
}

/*---------------------------------------------*\
|						|
|	outp_() Routine de bufferisation	|
|	des resultats et ecriture sur		|
|	disque					|
|						|
\*---------------------------------------------*/

static float	fvbuf[NVMAX];
static double	dvbuf[NVMAX];

void	outp_(void)
{
	int	i;
	double	dval;

	for (i=0;i<outp.nv;i++) {

		switch (outp.v_typ[i]) {

		case 'f' : /* SPARC : 32 bits floating point */
			dval = (double)(*(float *) outp.v_adr[i]);
			break;
		case 'd' : /* SPARC : 64 bits double precision floating point */
			dval = (double)(*(double *) outp.v_adr[i]);
			break;
		case 'b' : /* SPARC : 8 bits integer */
			dval = (double)(*(char *) outp.v_adr[i]);
			break;
		case 'B' : /* SPARC : 8 bits unsigned integer */
			dval = (double)(*(unsigned char *) outp.v_adr[i]);
			break;
		case 's' : /* SPARC : 16 bits integer */
			dval = (double)(*(short *) outp.v_adr[i]);
			break;
		case 'S' : /* SPARC : 16 bits unsigned integer */
			dval = (double)(*(unsigned short *) outp.v_adr[i]);
			break;
		case 'l' : /* SPARC : 32 bits integer */
			dval = (double)(*(long *) outp.v_adr[i]);
			break;
		case 'L' : /* SPARC : 32 bits unsigned integer */
			dval = (double)(*(unsigned long *) outp.v_adr[i]);
			break;
		case 'i' : /* default integer */
			dval = (double)(*(int *) outp.v_adr[i]);
			break;
		case 'I' : /* default unsigned integer */
			dval = (double)(*(unsigned int *) outp.v_adr[i]);
			break;
		}

		dvbuf[i] = (dval * outp.v_conv[i]);
		fvbuf[i] = (float)(dval * outp.v_conv[i]);
	}

	if(1)
		d_writ(fvbuf);
	else
		d_writ(dvbuf);
}

