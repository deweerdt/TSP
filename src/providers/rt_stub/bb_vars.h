/*!  \file 

$Id: bb_vars.h,v 1.1 2005-07-05 14:55:11 tsp_admin Exp $

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
Maintainer: tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Variables 

-----------------------------------------------------------------------
*/

#include <bb_simple.h>
#include "tsp_abs_types.h"


typedef struct mysim {
  double *DYN_0_d_qsat;		/* 4 elems */
  double *ORBT_0_d_possat_m;	/* 4 elems */
  double *ECLA_0_d_ecl_sol;
  double *ECLA_0_d_ecl_lune;
  double *POSA_0_d_DirSol;	/* 3 elems */
  double *POSA_0_d_DirLun;	/* 3 elems */
  double *Sequenceur_0_d_t_s;
}  mysim_t;

typedef struct mybench {
  /* Stats for distrib */
  double *simtime;
  double *rtime;
  double *delay;
  double *jitter;
  double *jitter_max;
  double *retard;
  /* What to realtime */
  unsigned long *affinity;
  int *sched;
  int *prio;
  int *rt_mode;
  int *it_mode;
  int *cfg_update;
  int *stopit;
} mybench_t;

typedef struct mytype {
  int     a;
  double  d;
  uint8_t byte;
} mytype_t;

typedef struct mydummy {
  uint32_t* display_level;
  int32_t* Toto;
  double* Titi;
  double* Tata;
  double* HugeArray;
  mytype_t* myvar;
  char*     astring;
} mydummy_t;

typedef struct mybbdata {
  mysim_t	sim;
  mybench_t	bench;
  mydummy_t	dummy;
} mybbdata_t;


mybbdata_t *bb_data_allocate(void);
int bb_data_publish (S_BB_T* mybb, mybbdata_t *data);
int bb_data_propagate (S_BB_T* mybb, mybbdata_t *data);
