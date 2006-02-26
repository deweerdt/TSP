/*

$Id: bb_vars.c,v 1.2 2006-02-26 13:36:06 erk Exp $

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

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <math.h>
#include <libgen.h>
#include <string.h>
#include <bb_core.h>
#include <bb_simple.h>
#include "tsp_abs_types.h"

#include "bb_vars.h"
#include "rtc_util.h"


#define BB_PUBLISH(bb_name,name,base,instance,bb_type,c_type,dim) \
  data->name = (c_type*) bb_simple_publish(bb_name, #name, (base), instance, bb_type, sizeof(c_type), dim); \
  printf ("INFO : BB_PUBLISH %s[%d] type %s\n", #name, dim, #c_type); \
  for (i=0;i<dim;++i) { \
    data->name[i] = (c_type)0; \
  }

#define BIG_SIZE 100

mybbdata_t *bb_data_allocate(void)
{
  static mybbdata_t mydata;
  return & mydata;
}


/* Publish data in the BB */
/**************************/ 
int bb_data_publish (S_BB_T* mybb, mybbdata_t *data)
{
  int i;
  /* The S/C sim part */
  BB_PUBLISH(mybb, sim.DYN_0_d_qsat, "", -1, E_BB_DOUBLE, double, 4);
  BB_PUBLISH(mybb, sim.ORBT_0_d_possat_m, "", -1,  E_BB_DOUBLE, double, 3);
  BB_PUBLISH(mybb, sim.ECLA_0_d_ecl_sol, "", -1,  E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, sim.ECLA_0_d_ecl_lune, "", -1,  E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, sim.POSA_0_d_DirSol, "", -1,  E_BB_DOUBLE, double, 3);
  BB_PUBLISH(mybb, sim.POSA_0_d_DirLun, "", -1,  E_BB_DOUBLE, double, 3);
  BB_PUBLISH(mybb, sim.Sequenceur_0_d_t_s, "", -1,  E_BB_DOUBLE, double, 1);
 
  /* The bench part */
  BB_PUBLISH(mybb, bench.simtime, "", -1, E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, bench.rtime, "", -1, E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, bench.delay, "", -1, E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, bench.jitter, "", -1, E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, bench.jitter_max, "", -1, E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, bench.retard, "", -1, E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, bench.affinity, "", -1, E_BB_UINT64, unsigned long, 1);
  BB_PUBLISH(mybb, bench.prio, "", -1, E_BB_INT32, int, 1);
  BB_PUBLISH(mybb, bench.sched, "", -1, E_BB_INT32, int, 1);
  BB_PUBLISH(mybb, bench.rt_mode, "", -1, E_BB_INT32, int, 1);
  BB_PUBLISH(mybb, bench.it_mode, "", -1, E_BB_INT32, int, 1);
  BB_PUBLISH(mybb, bench.cfg_update, "", -1, E_BB_INT32, int, 1);
  *data->bench.affinity = 0x1;
  *data->bench.rt_mode = 1;  /* 0 = NRT, 1  RT mode */
  *data->bench.it_mode = 0;  /* 0 = Polling on date, 1=IT , 2=IT blocking IO */ 
  *data->bench.stopit = 0;
  *data->bench.prio = 50;
  *data->bench.sched = 0;
  *data->bench.cfg_update = 1; /* Ned to do something */
  

  /* The dummy part */
  BB_PUBLISH(mybb, dummy.display_level, "", -1, E_BB_UINT32, uint32_t, 1);
  BB_PUBLISH(mybb, dummy.astring, "", -1, E_BB_CHAR, char, 20);
  BB_PUBLISH(mybb, dummy.Toto, "", -1, E_BB_INT32, uint32_t, 3);
  BB_PUBLISH(mybb, dummy.Titi, "", -1, E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, dummy.Tata, "", -1, E_BB_DOUBLE, double, 9);
  BB_PUBLISH(mybb, dummy.HugeArray, "", -1, E_BB_DOUBLE, double, BIG_SIZE);

  data->dummy.myvar = (mytype_t*) bb_simple_publish(mybb,"dummy.mytype_t_var","",-1, E_BB_USER, sizeof(mytype_t),1);
  data->dummy.myvar->a = 1;
  data->dummy.myvar->d = 3.14159;
  data->dummy.myvar->byte = 0xFF;

  strncpy(data->dummy.astring,"A little string",20);
  for (i=0;i<3;++i) {
    data->dummy.Toto[i] = i;
  }
  *data->dummy.Titi = 3.14159;
  for (i=0;i<9;++i) {
    data->dummy.Tata[i] = cos(*data->dummy.Titi/(i+1));
  }
  for (i=0;i<BIG_SIZE;++i) {
    data->dummy.HugeArray[i] = cos(*data->dummy.Titi/(i+1));
  }

  return 0;
}

/* Evoluate data in the BB */
/**************************/ 
int bb_data_propagate (S_BB_T* mybb, mybbdata_t *data)
{
  static int i=0;
  double dq1, dq2, teta, d=7e6 /*km*/;

  /* Definitions des variables SIM */
  *data->sim.Sequenceur_0_d_t_s = *data->sim.Sequenceur_0_d_t_s + 0.01; /* 10 Hz Simu */
  teta = *data->sim.Sequenceur_0_d_t_s*2*3.14/100; /* 100s for a complete turn around */
  data->sim.ORBT_0_d_possat_m[0] = d*cos(teta); /* simple circular orbit */
  data->sim.ORBT_0_d_possat_m[1] = d*sin(teta); /* do better later */
  data->sim.ORBT_0_d_possat_m[2] = d*0.0; /* force Z axis is null */
  dq1 = cos(teta)/4;
  dq2 = sin(teta)/4;
  data->sim.DYN_0_d_qsat[0] = 0.7+dq1;
  data->sim.DYN_0_d_qsat[1] = 0.3-dq1;
  data->sim.DYN_0_d_qsat[2] = 0.3+dq2;
  data->sim.DYN_0_d_qsat[3] = -0.2-dq2;
  data->sim.POSA_0_d_DirLun[1] = 0.4; /* e6 */
  data->sim.POSA_0_d_DirSol[0] = 0.15; /* e3 */
  *data->sim.ECLA_0_d_ecl_lune = 1;
  *data->sim.ECLA_0_d_ecl_sol = 1;
  
  /* Definitions des variables Dummy */
  data->dummy.Tata[0] = sin((2.0*i)/180.0);
  data->dummy.Tata[1] = cos((2.0*i)/180.0);
  data->dummy.Toto[0] = i % 1000;
  if (0==(i % 100)) {
    data->dummy.Toto[1] = -data->dummy.Toto[1];
  }
  
  bb_simple_synchro_go(mybb,BB_SIMPLE_MSGID_SYNCHRO_COPY);
  if (*data->dummy.display_level & 0x4) {
    printf("OK.\n");
  }
  
  /* 	printf("Synchro Wait..."); */
  /* 	fflush(stdout); */
  /* 	bbntb_synchro_wait(BBNTB_MSGID_SYNCHRO_COPY_ACK); */
  /* 	printf("OK.\n"); */
  /* 	fflush(stdout); */

  if (*data->bench.cfg_update)
    {
      if (*data->bench.rt_mode)
	rt_memory_lock(0);

      set_real_time_priority (0 /*myself*/, *data->bench.affinity, *data->bench.sched, *data->bench.prio );
      *data->bench.cfg_update = 0;
    }

  return 0;
}
