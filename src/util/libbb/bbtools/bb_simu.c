/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bbtools/bb_simu.c,v 1.14 2008-05-02 14:55:34 deweerdt Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

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
Component : BlackBoard

-----------------------------------------------------------------------

Purpose   : BlackBoard Idiom implementation

-----------------------------------------------------------------------
 */
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stddef.h>
#include <math.h>
#include <libgen.h>
#include <string.h>
#include <signal.h>

#include <bb_core.h>
#include <bb_simple.h>
#include <tsp_abs_types.h>

typedef struct MyInsiderType {
	int32_t ai[2];
	double f;
} MyInsiderType_t;

typedef struct MyType {
	int32_t a;
	double d;
	uint8_t byte;
	MyInsiderType_t insider;
	MyInsiderType_t insider_array[2];
} MyType_t;

double *DYN_0_d_qsat;		/* 4 elems */
double *ORBT_0_d_possat_m;	/* 4 elems */
double *ECLA_0_d_ecl_sol;
double *ECLA_0_d_ecl_lune;
double *POSA_0_d_DirSol;	/* 3 elems */
double *POSA_0_d_DirLun;	/* 3 elems */
double *Sequenceur_0_d_t_s;

#define BB_PUBLISH(bb_name,name,base,instance,bb_type,c_type,dim) \
  name = (c_type*) bb_simple_publish(bb_name, #name, (base), instance, bb_type, sizeof(c_type), dim); \
  printf ("INFO : BB_PUBLISH %s[%d] type %s\n", #name, dim, #c_type); \
  for (i=0;i<dim;++i) { \
    name[i] = (c_type)0; \
  }

static S_BB_T *mybb;
static void sigint_handler(int sig)
{
	bb_destroy(&mybb);
}

int main(int argc, char **argv)
{

	/* Définitions des variables */

	int retcode = 0;
	int data_size;
	int n_data;
	uint32_t *display_level;
	int32_t *Toto;
	int8_t *aint8;
	uint8_t *auint8;
	double *Titi;
	double *Tata;
	double *HugeArray;
	int i;
	int synchro = 0;
	MyType_t *myvar;
	char *astring;
	int opt;

	/*
	 * FIXME we should add
	 *   -a for acknowledged BB mode
	 */
#define USAGE_STRING "Usage: %s [-s] [-x]\n\
\t\t-s: use synchro\n\
\t\t-x: destroy bb on exit\n"
	while ((opt = getopt(argc, argv, "sxh")) != -1) {
		switch (opt) {
		case 's':
			synchro = 1;
			break;
		case 'x':
			signal(SIGINT, sigint_handler);
			break;
		case 'h':
		default:
			fprintf(stderr, USAGE_STRING, argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	/* Creation BB */
	n_data = 1000;
	data_size = n_data * 8 + 500 * 30 * 4 + 200000 * 8;
	if (BB_NOK == bb_create(&mybb, basename(argv[0]), n_data, data_size)) {
		bb_attach(&mybb, basename(argv[0]));
	}

	/* Publish data in the BB */
	display_level = (uint32_t *) bb_simple_publish(mybb, "display_level",
						       basename(argv[0]), -1,
						       E_BB_UINT32, sizeof(uint32_t),
						       1);
	*display_level = 0;

	aint8 = (int8_t *) bb_simple_publish(mybb, "int8", basename(argv[0]),
					     -1, E_BB_INT8, sizeof(int8_t), 2);
	auint8 = (uint8_t *) bb_simple_publish(mybb, "uint8", basename(argv[0]),
					       -1, E_BB_UINT8, sizeof(uint8_t),
					       2);

	myvar = (MyType_t *) bb_simple_publish(mybb, "MyType_t_var",
					       basename(argv[0]), -1,
					       E_BB_USER, sizeof(MyType_t), 1);

	/* Now we may alias publish the structure in order to be able 
	 * to distribute the component of the structure using TSP
	 * Note that most of the time we may ignore the return value
	 * since the variable is usable trough the structure itself
	 */
	bb_simple_alias_publish(mybb, "a", "MyType_t_var",
				basename(argv[0]), -1, E_BB_INT32,
				sizeof(int32_t), 1, offsetof(MyType_t, a));
	bb_simple_alias_publish(mybb, "d", "MyType_t_var",
				basename(argv[0]), -1, E_BB_DOUBLE,
				sizeof(double), 1, offsetof(MyType_t, d));
	bb_simple_alias_publish(mybb, "byte", "MyType_t_var",
				basename(argv[0]), -1, E_BB_UINT8,
				sizeof(uint8_t), 1, offsetof(MyType_t,
							     byte));

	/* You may specify USER type in USER type */
	bb_simple_alias_publish(mybb, "insider", "MyType_t_var",
				basename(argv[0]), -1, E_BB_USER,
				sizeof(MyInsiderType_t), 1,
				offsetof(MyType_t, insider));

	/* Now the (in)famous structure in structure i.e. alias of alias example */
	bb_simple_alias_publish(mybb, "ai", "MyType_t_var.insider",
				basename(argv[0]), -1, E_BB_INT32,
				sizeof(int32_t), 2,
				offsetof(MyInsiderType_t, ai));

	bb_simple_alias_publish(mybb, "f", "MyType_t_var.insider",
				basename(argv[0]), -1, E_BB_DOUBLE,
				sizeof(double), 1,
				offsetof(MyInsiderType_t, f));

	/* You may even specify ARRAY of USER type in USER type */
	bb_simple_alias_publish(mybb, "insider_array", "MyType_t_var",
				basename(argv[0]), -1, E_BB_USER,
				sizeof(MyInsiderType_t), 2,
				offsetof(MyType_t, insider_array));

	/* Then you should re-specify alias of alias where target alias is an array */
	bb_simple_alias_publish(mybb, "ai", "MyType_t_var.insider_array",
				basename(argv[0]), -1, E_BB_INT32,
				sizeof(int32_t), 2,
				offsetof(MyInsiderType_t, ai));

	bb_simple_alias_publish(mybb, "f", "MyType_t_var.insider_array",
				basename(argv[0]), -1, E_BB_DOUBLE,
				sizeof(double), 1,
				offsetof(MyInsiderType_t, f));
	myvar->a = 1;
	myvar->d = 3.14159;
	myvar->byte = 0xFF;

	astring =
	    (char *) bb_simple_publish(mybb, "astring", basename(argv[0]),
				       -1, E_BB_CHAR, sizeof(char), 20);
	strncpy(astring, "A little string", 20);

	Toto =
	    (int32_t *) bb_simple_publish(mybb, "Toto", basename(argv[0]),
					  1, E_BB_INT32, sizeof(int32_t),
					  3);
	for (i = 0; i < 3; ++i) {
		Toto[i] = i;
	}
	for (i = 0; i < 3; ++i) {
		printf("@Toto = %p, Toto[%d] = %d\n",
		       &Toto[i], i, Toto[i]);
	}

	Titi =
	    (double *) bb_simple_publish(mybb, "Titi", basename(argv[0]),
					 1, E_BB_DOUBLE, sizeof(double),
					 1);
	*Titi = 3.14159;

	printf("@Titi = %p, Titi = %f\n", Titi, *Titi);

	Tata =
	    (double *) bb_simple_publish(mybb, "Tata", basename(argv[0]),
					 1, E_BB_DOUBLE, sizeof(double),
					 9);
	for (i = 0; i < 9; ++i) {
		Tata[i] = cos(*Titi / (i + 1));
	}

#undef HUGE_ARRAY
#ifdef HUGE_ARRAY
#define BIG_SIZE 200000
#else
#define BIG_SIZE 250
#endif
	HugeArray =
	    (double *) bb_simple_publish(mybb, "HugeArray",
					 basename(argv[0]), 1, E_BB_DOUBLE,
					 sizeof(double), BIG_SIZE);
	if (NULL == HugeArray) {
		printf("Cannot allocate HUGE array of size %d aborting...\n",
		       BIG_SIZE);
		return -1;
	}
	for (i = 0; i < BIG_SIZE; ++i) {
		HugeArray[i] = cos(*Titi / (i + 1));
	}

	BB_PUBLISH(mybb, DYN_0_d_qsat, "", -1, E_BB_DOUBLE, double, 4);
	BB_PUBLISH(mybb, ORBT_0_d_possat_m, "", -1, E_BB_DOUBLE, double, 3);
	BB_PUBLISH(mybb, ECLA_0_d_ecl_sol, "", -1, E_BB_DOUBLE, double, 1);
	BB_PUBLISH(mybb, ECLA_0_d_ecl_lune, "", -1, E_BB_DOUBLE, double, 1);
	BB_PUBLISH(mybb, POSA_0_d_DirSol, "", -1, E_BB_DOUBLE, double, 3);
	BB_PUBLISH(mybb, POSA_0_d_DirLun, "", -1, E_BB_DOUBLE, double, 3);
	BB_PUBLISH(mybb, Sequenceur_0_d_t_s, "", -1, E_BB_DOUBLE, double, 1);

	for (i = 0; i < 3; ++i) {
		printf("Toto[%d] = %d\n", i, Toto[i]);
	}
	printf("Titi = %f\n", *Titi);
	for (i = 0; i < 9; ++i) {
		printf("Tata[%d] = %f\n", i, Tata[i]);
	}

	i = 0;
	sleep(5);
	while (i < 2000000) {
		double dq1, dq2, teta, d = 7e6 /*km */ ;
		*Sequenceur_0_d_t_s = *Sequenceur_0_d_t_s + 0.01;	/* 10 Hz Simu */
		teta = *Sequenceur_0_d_t_s * 2 * 3.14 / 100;	/* 100s for a complete turn around */
		ORBT_0_d_possat_m[0] = d * cos(teta);	/* simple circular orbit */
		ORBT_0_d_possat_m[1] = d * sin(teta);	/* do better later */
		ORBT_0_d_possat_m[2] = d * 0.0;	/* force Z axis is null */
		dq1 = cos(teta) / 4;
		dq2 = sin(teta) / 4;
		DYN_0_d_qsat[0] = 0.7 + dq1;
		DYN_0_d_qsat[1] = 0.3 - dq1;
		DYN_0_d_qsat[2] = 0.3 + dq2;
		DYN_0_d_qsat[3] = -0.2 - dq2;
		POSA_0_d_DirLun[1] = 0.4;	/* e6 */
		POSA_0_d_DirSol[0] = 0.15;	/* e3 */
		*ECLA_0_d_ecl_lune = 1;
		*ECLA_0_d_ecl_sol = 1;


		Tata[0] = sin((2.0 * i) / 180.0);
		Tata[1] = cos((2.0 * i) / 180.0);
		Toto[0] = i % 1000;
		if (0 == (i % 100)) {
			Toto[1] = -Toto[1];
		}
		if ((i % 2) == 0) {
			bb_lock(mybb);
			*Titi += 1;
			if (*display_level & 0x1) {
				printf("Titi = %f\n", *Titi);
			}
			if (*display_level & 0x2) {
				printf("BB locked <i=%d>\n", i);
			}
		} else {
			*Titi += 1;
			if (*display_level & 0x1) {
				printf("Titi = %f\n", *Titi);
			}
			bb_unlock(mybb);
			if (*display_level & 0x2) {
				printf("BB unlocked\n");
			}
			if (synchro) {
				if (*display_level & 0x4) {
					printf("Synchro GO...");
					fflush(stdout);
				}
				bb_simple_synchro_go(mybb, BB_SIMPLE_MSGID_SYNCHRO_COPY);
				if (*display_level & 0x4) {
					printf("OK.\n");
				}
#if 0
 	printf("Synchro Wait...");
 	fflush(stdout);
 	bbntb_synchro_wait(BBNTB_MSGID_SYNCHRO_COPY_ACK);
 	printf("OK.\n");
 	fflush(stdout);
#endif
			}
		}
		usleep(10000);
		if (++i % 100 == 0) {
			printf(".");
			fflush(stdout);
		}
	}

	/* Destroy BB */
	retcode = bb_destroy(&mybb);

	return (retcode);
}
