/*

$Id: client_group.c,v 1.9 2008-02-05 18:54:09 rhdv Exp $

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Simple consummer for testing groups configuration

-----------------------------------------------------------------------
*/

#include <stdio.h>
#include "tsp_consumer.h"

/* Allow us to cheat with groups internal and hidden structures*/
#include "tsp_group_data.h"

TSP_groups_t TSP_test_get_groups(TSP_provider_t provider);

/*µS*/
#define TSP_NANOSLEEP_PERIOD_US (200*1000)

/* Number of samples  that will be counted before the data check test pass */
#define TSP_TEST_COUNT_SAMPLES 200000



struct data_test_t
{
  int index;
  int period;
  int phase;
  char* name;

};

typedef struct data_test_t data_test_t;

/* requested samples */
static data_test_t data_test[] = {
  {10,2,0,"Symbol10"},
  {20,3,2,"Symbol20"},
  {30,5,3,"Symbol30"},
  {-2,-2,-2,NULL}
};

struct group_test_t 
{

  int index[10];

};

typedef struct group_test_t group_test_t;

/* Calculated group numbers */
static group_test_t group_test[] = {
		{{ 10, -1, -1 , -1}}, /*00*/
	 /*01*/ {{ -1, -1, -1 , -1}},
	 /*02*/ {{ 10, 20, -1 , -1}},
	 /*03*/ {{ 30, -1, -1 , -1}},
	 /*04*/ {{ 10, -1, -1 , -1}},
	 /*05*/ {{ 20, -1, -1 , -1}},
	 /*06*/ {{ 10, -1, -1 , -1}},
	 /*07*/ {{ -1, -1, -1 , -1}},
	 /*08*/ {{ 10, 20, 30 , -1}},
	 /*09*/ {{ -1, -1, -1 , -1}},
	 /*10*/ {{ 10, -1, -1 , -1}},
	 /*11*/ {{ 20, -1, -1 , -1}},
	 /*12*/ {{ 10, -1, -1 , -1}},
	 /*13*/ {{ 30, -1, -1 , -1}},
	 /*14*/ {{ 10, 20, -1 , -1}},
	 /*15*/ {{ -1, -1, -1 , -1}},
	 /*16*/ {{ 10, -1, -1 , -1}},
	 /*17*/ {{ 20, -1, -1 , -1}},
	 /*18*/ {{ 10, 30, -1 , -1}},
	 /*19*/ {{ -1, -1, -1 , -1}},
	 /*20*/ {{ 10, 20, -1 , -1}},
	 /*21*/ {{ -1, -1, -1 , -1}},
	 /*22*/ {{ 10, -1, -1 , -1}},
	 /*23*/ {{ 20, 30, -1 , -1}}, 
	 /*24*/ {{ 10, -1, -1 , -1}},
	 /*25*/ {{ -1, -1, -1 , -1}},
	 /*26*/ {{ 10, 20, -1 , -1}},
	 /*27*/ {{ -1, -1, -1 , -1}},
	 /*28*/ {{ 10, 30, -1 , -1}},
	 /*29*/ {{ 20, -1, -1 , -1}},
	 /*30*/ {{ -2, -2, -2 , -2}}   
};


int main(int argc, char *argv[]){

  TSP_sample_symbol_info_list_t symbols;

  int i, j;
  int nb_providers;
  int period=0;
  char* name;
  int test_mode;


  TSP_group_table_t* groups;

  TSP_provider_t* providers;

  int requested_nb;
  int group_nb;
 
  printf ("#=========================================================#\n");
  printf ("# Launching <test_group> for test purpose #\n");
  printf ("#=========================================================#\n");
 

  if(TSP_STATUS_OK!=TSP_consumer_init(&argc, &argv))
    {
      STRACE_ERROR("TSP init failed");
      return -1;
    }

  if (argc>2)
    {   
      name = argv[1];
      period = atoi (argv[2]);
      /* Anything after name and perdio --> test mode */
      test_mode = (argc>3) ? TRUE : FALSE;
    }
  else
    {
      STRACE_ERROR("USAGE %s : server period", argv[0]);
      return -1;
    }

  
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 001 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  TSP_consumer_connect_all(name,&providers, &nb_providers);
  if(nb_providers > 0)
    {
      for( i = 0 ; i<nb_providers ; i++)
	{
	  const char* info = TSP_consumer_get_connected_name(providers[i]) ;
	  STRACE_INFO("Server Nb %d, info = '%s'", i,info);
	  
	  /* Check name */
	  if(strcmp(info, "StubbedServer"))
	    {
	      STRACE_ERROR("Server name corrupted");
	      return -1;
	    }
	  
	}
    }
  else
    {
      STRACE_ERROR("Unable to find any provider for host");
      return -1;
    }




  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 002 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Le 1er provider existe puisqu'il y en a au moins 1 */

  if(TSP_STATUS_OK!=TSP_consumer_request_open(providers[0], 0, 0)) {
    STRACE_ERROR("TSP_request_provider_open failed");
    return -1;
  }
  

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_information(providers[0]))
    {
      STRACE_ERROR("TSP_request_provider_information failed");
      return -1;
    }
  
  /* How many symbols do we want to send ? */
  for(requested_nb = 0; data_test[requested_nb].index != (-2); requested_nb++);
  STRACE_INFO("Total number of requested symbols = %d", requested_nb);


  symbols.TSP_sample_symbol_info_list_t_len = requested_nb;
  symbols.TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*)calloc(requested_nb, sizeof(TSP_sample_symbol_info_t));
  
  /* Initialize data of sampling */
  for(i = 0 ; i <requested_nb ; i++)
    {
      symbols.TSP_sample_symbol_info_list_t_val[i].phase = data_test[i].phase ;
      symbols.TSP_sample_symbol_info_list_t_val[i].period = data_test[i].period ;
      symbols.TSP_sample_symbol_info_list_t_val[i].name = data_test[i].name ;
    }
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 004 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_sample(providers[0], &symbols))
    {
      STRACE_ERROR("TSP_request_provider_sample failed");
      return -1;
    }


  /* Cheating... we can do that coz of included   tsp_group_data.h */
  groups = (TSP_group_table_t*)TSP_test_get_groups(providers[0]);


  /* How many symbols do we want to send ? */
  for(group_nb = 0; group_test[group_nb].index[0] != (-2); group_nb++);

  if( group_nb != groups->table_len )
    {
      STRACE_ERROR("Wrong groups total number");
      return -1;
    }

  /* Compare groups size */
  {
    for(i = 0 ; i < group_nb ; i++)
      {
	/* What size are we expecting for group i?*/
	int size;
	for(size = 0; group_test[i].index[size] != (-1); size++);
	if ( size != groups->groups[i].group_len )
	  {
	    STRACE_ERROR("Wrong groups size");
	    return -1;
	  }
      }
  }

  /* Compare groups internals */
  for(i = 0 ; i < group_nb ; i++)
    {
      /* What size are we expecting for group i?*/
      for( j = 0; j < groups->groups[i].group_len ; j++)
	{
	  if (  group_test[i].index[j] != groups->groups[i].items[j].provider_global_index )
	    {
	      STRACE_ERROR("Wrong provider global index");
	      return -1;
	    }
	}
    }

  STRACE_TEST("STAGE 002 | STEP 001 : PASSED");
  printf ("#=========================================================#\n");
  printf ("# End <test_group> OK #\n");
  printf ("#=========================================================#\n");
 
  return 0;
}
