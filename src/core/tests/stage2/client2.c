#include "tsp_sys_headers.h"
#include <math.h>

#include "tsp_prjcfg.h"
#include "tsp_consumer.h"
#include "tsp_time.h"

/* Allow us to cheat with groups internal and hidden structures*/
#include "tsp_group_data.h"

/*µS*/
#define TSP_NANOSLEEP_PERIOD_US (200*1000)

/* Number of samples  that will be counted before the data check test pass */
#define TSP_TEST_COUNT_SAMPLES 200000



struct data_test_t
{
  int index;
  int period;
  int phase;

};

typedef struct data_test_t data_test_t;

/* requested samples */
static data_test_t data_test[] = {
  {10,2,0},
  {20,3,2},
  {30,5,3},
  {-2,-2,-2}
};

struct group_test_t 
{

  int index[10];

};

typedef struct group_test_t group_test_t;

/* Calculated group numbers */
static group_test_t group_test[] = {
  /*00*/ { 10, -1, -1 , -1},
	 /*01*/ { -1, -1, -1 , -1},
	 /*02*/ { 10, 20, -1 , -1},
	 /*03*/ { 30, -1, -1 , -1},
	 /*04*/ { 10, -1, -1 , -1},
	 /*05*/ { 20, -1, -1 , -1},
	 /*06*/ { 10, -1, -1 , -1},
	 /*07*/ { -1, -1, -1 , -1},
	 /*08*/ { 10, 20, 30 , -1},
	 /*09*/ { -1, -1, -1 , -1},
	 /*10*/ { 10, -1, -1 , -1},
	 /*11*/ { 20, -1, -1 , -1},
	 /*12*/ { 10, -1, -1 , -1},
	 /*13*/ { 30, -1, -1 , -1},
	 /*14*/ { 10, 20, -1 , -1},
	 /*15*/ { -1, -1, -1 , -1},
	 /*16*/ { 10, -1, -1 , -1},
	 /*17*/ { 20, -1, -1 , -1},
	 /*18*/ { 10, 30, -1 , -1},
	 /*19*/ { -1, -1, -1 , -1},
	 /*20*/ { 10, 20, -1 , -1},
	 /*21*/ { -1, -1, -1 , -1},
	 /*22*/ { 10, -1, -1 , -1},
	 /*23*/ { 20, 30, -1 , -1}, 
	 /*24*/ { 10, -1, -1 , -1},
	 /*25*/ { -1, -1, -1 , -1},
	 /*26*/ { 10, 20, -1 , -1},
	 /*27*/ { -1, -1, -1 , -1},
	 /*28*/ { 10, 30, -1 , -1},
	 /*29*/ { 20, -1, -1 , -1},
	 /*30*/ { -2, -2, -2 , -2},

   
};


int main(int argc, char *argv[]){

  SFUNC_NAME(main);


  TSP_answer_sample_t* ans_sample;
  TSP_request_sample_t req_sample;

  int i, j, count=0;
  int nb_providers;
  int period=0;
  char* name;
  int count_samples = 0;
  char symbol_buf[50];
  int test_ok = TRUE;
  int test_mode;


  TSP_group_table_t* groups;

  TSP_provider_t* providers;

  int requested_nb;
  int group_nb;
 
  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

  if (argc>2)
    {   
      name = argv[1];
      period = atoi (argv[2]);
      /* Anything after name and perdio --> test mode */
      test_mode = (argc>3) ? TRUE : FALSE;
    }
  else
    {
      STRACE_ERROR(("USAGE %s : server period\n", argv[0]));
      return -1;
    }

  
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 001 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  TSP_open_all_provider(name,&providers, &nb_providers);
  if(nb_providers > 0)
    {
      for( i = 0 ; i<nb_providers ; i++)
	{
	  const TSP_otsp_server_info_t* info_struct = TSP_get_provider_simple_info(providers[i]) ;
	  STRACE_INFO(("Server Nb %d, info = '%s'", i, info_struct->info));
	  
	  /* Check name */
	  if(strcmp(info_struct->info, "ServeurPetitScarabe"))
	    {
	      STRACE_ERROR(("Serveur name corrupted"));
	      STRACE_TEST(("STAGE 002 | STEP 001 : FAILED"));
	      return -1;
	    }
	  
	}
    }
  else
    {
      STRACE_ERROR(("Unable to find any provider for host"));
      return -1;
    }




  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 002 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Le 1er provider existe puisqu'il y en a au moins 1 */

  if(!TSP_request_provider_open(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_open failed"));
      STRACE_TEST(("STAGE 002 | STEP 001 : FAILED"));
      return -1;
    }
  

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_request_provider_information(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      STRACE_TEST(("STAGE 002 | STEP 001 : FAILED"));
      return -1;
    }
  
  /* How many symbols do we want to send ? */
  for(requested_nb = 0; data_test[requested_nb].index != (-2); requested_nb++);
  STRACE_INFO(("Total number of requested symbols = %d", requested_nb));


  req_sample.symbols.TSP_sample_symbol_info_list_t_len = requested_nb;
  req_sample.symbols.TSP_sample_symbol_info_list_t_val = 
    (TSP_sample_symbol_info_t*)calloc(requested_nb, sizeof(TSP_sample_symbol_info_t));
  
  /* Initialize data of sampling */
  for(i = 0 ; i < req_sample.symbols.TSP_sample_symbol_info_list_t_len ; i++)
    {
      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].name = "";
      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].phase = data_test[i].phase ;
      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].period = data_test[i].period ;
      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index = data_test[i].index ;
    }
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 004 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_request_provider_sample(&req_sample, providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      STRACE_TEST(("STAGE 002 | STEP 001 : FAILED"));
      return -1;
    }


  /* Cheating... we can do that coz of included   tsp_group_data.h */
  groups = (TSP_group_table_t*)TSP_test_get_groups(providers[0]);


  /* print groups*/
  /*
    for(i = 0 ; i < groups->table_len ; i++)
    {
    printf("\nN=%d|", i);  
    for( j = 0 ; j <  groups->groups[i].group_len ; j++)
    {
    printf("%d,", groups->groups[i].items[j].provider_global_index );
    }
    }
  
    printf("\n");
  */
 
  /* How many symbols do we want to send ? */
  for(group_nb = 0; group_test[group_nb].index[0] != (-2); group_nb++);

  if( group_nb != groups->table_len )
    {
      STRACE_ERROR(("Wrong groups total number"));
      STRACE_TEST(("STAGE 002 | STEP 001 : FAILED"));
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
	    STRACE_ERROR(("Wrong groups size"));
	    STRACE_TEST(("STAGE 002 | STEP 001 : FAILED"));
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
	      STRACE_ERROR(("Wrong provider global index"));
	      STRACE_TEST(("STAGE 002 | STEP 001 : FAILED"));
	      return -1;
	    }
	}
     

	
    }

  STRACE_TEST(("STAGE 002 | STEP 001 : PASSED"));
  return 0;
}
