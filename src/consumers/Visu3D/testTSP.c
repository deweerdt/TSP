/*

$Id: testTSP.c,v 1.4 2006-04-24 22:17:47 erk Exp $

-----------------------------------------------------------------------

Visu3D - 3D Software Visualisation.

Copyright (c) 2004 Olivier Monaret

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
Maintainer : olivier.monaret
Component : Visu3D :: Main Visu3D

-----------------------------------------------------------------------

Purpose   : Main Visu3D

-----------------------------------------------------------------------
 */
/****************************************************************************
Visu3D
Authors: Eric Noulard - Olivier Monaret

****************************************************************************/


#include "glScene.h"//is api OpenGL ; it uses GL.H and GLU.H
#include <GL/glut.h>//glut is use only in this file

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

#include <signal.h>
#include <stdlib.h>
#include "tsp_abs_types.h"
#include "tsp_consumer.h"
#include <pthread.h>

#define MAX_VAR_NAME_SIZE 256
#define NBINFO 16


/* Constants for 3ds space*/
#define RADIUS_EARTH 6378.0
#define RADIUS_SAT   4.0
#define RADIUS_PAN   8.0
#define RADIUS_SUN   200.0 // real: 696000 km 
#define RADIUS_MOON  400.0 // real: 1700 km  
#define RADIUS_STARS 19000.0
#define DIST_EARTH_MOON 384000.0 // real: 384000 km
#define DIST_EARTH_SUN  150000000.0 // real: 150.000.000 km
/**************************/



/*   TSP config    */
static void 
my_sighandler(int signum) {
  fprintf(stderr,"tsp_ascii_writer::Captured signal<%d>\n",signum);
  fflush(stderr);
}
  struct sigaction my_action;
  struct sigaction old_action;

/**********************************************************************/
/* Variables 3d*/

glSceneStruct MyScene;
int n3ds_earth;
int n3ds_moon;
int n3ds_sun;
int n3ds_sat;
int n3ds_pan;//satellite panels
int light_sun;
int mono_light_sun_shining;
int light_ambient;

float Position_Camera[]={0.0,0.0,0.0}; 
float Position_Earth[]={0.0,0.0,0.0}; 
float Position_Moon[]={10000.0,2000.0,2000.0}; 
float Position_Sun[]={-20000.0,0.0,0.0}; 
float Position_Sat[]={0.0,0.0,0.0}; 
float Position_Pan[]={0.0,0.0,0.0}; 
float Quaternion_Sat[]={1.0,0.0,0.0,0.0}; 
float Dist_Camera_Sat=20.0;
int eclipsesun,eclipsemoon;

float MatCamera[]={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}; 

int   main_window;

/***********************************************************************/
/* Variables TSP*/

static TSP_provider_t* myproviders = NULL;
char tsp_provider_url[200];
char config_file[100];
TSP_sample_symbol_info_list_t  symbol_list;
const TSP_sample_symbol_info_list_t*  symbols;
   int complete_line = 0;
  int             new_sample;
  TSP_sample_t    sample;
  float lastinfo[NBINFO];

/**************************************************************************/
void myGlutKeyboard(unsigned char Key, int x, int y)
{
  float Mat[12];
  float MatCamera2[16];
  MatCamera2[0] = MatCamera[0];
  MatCamera2[1] = MatCamera[1];
  MatCamera2[2] = MatCamera[2];
  MatCamera2[3] = MatCamera[3];
  MatCamera2[4] = MatCamera[4];
  MatCamera2[5] = MatCamera[5];
  MatCamera2[6] = MatCamera[6];
  MatCamera2[7] = MatCamera[7];
  MatCamera2[8] = MatCamera[8];
  MatCamera2[9] = MatCamera[9];
  MatCamera2[10] = MatCamera[10];
  MatCamera2[11] = MatCamera[11];
  MatCamera2[12] = MatCamera[12];
  MatCamera2[13] = MatCamera[13];
  MatCamera2[14] = MatCamera[14];
  MatCamera2[15] = MatCamera[15];

  switch(Key)
  {
  case '4':
    Mat[0]=0.984808;   Mat[1]=0.0;   Mat[2]=-0.173648;
    Mat[4]=0.0;        Mat[5]=1.0;   Mat[6]=0.0;
    Mat[8]=0.173648;   Mat[9]=0.0;   Mat[10]=0.984808;
    MatCamera[0]=MatCamera2[0]* Mat[0] + MatCamera2[1]* Mat[4] + MatCamera2[2]* Mat[8];
    MatCamera[1]=MatCamera2[0]* Mat[1] + MatCamera2[1]* Mat[5] + MatCamera2[2]* Mat[9];
    MatCamera[2]=MatCamera2[0]* Mat[2] + MatCamera2[1]* Mat[6] + MatCamera2[2]* Mat[10];
    MatCamera[4]=MatCamera2[4]* Mat[0] + MatCamera2[5]* Mat[4] + MatCamera2[6]* Mat[8];
    MatCamera[5]=MatCamera2[4]* Mat[1] + MatCamera2[5]* Mat[5] + MatCamera2[6]* Mat[9];
    MatCamera[6]=MatCamera2[4]* Mat[2] + MatCamera2[5]* Mat[6] + MatCamera2[6]* Mat[10];
    MatCamera[8]=MatCamera2[8]* Mat[0] + MatCamera2[9]* Mat[4] + MatCamera2[10]* Mat[8];
    MatCamera[9]=MatCamera2[8]* Mat[1] + MatCamera2[9]* Mat[5] + MatCamera2[10]* Mat[9];
    MatCamera[10]=MatCamera2[8]* Mat[2] + MatCamera2[9]* Mat[6] + MatCamera2[10]* Mat[10];
    break;
  case '6':
    Mat[0]=0.984808;   Mat[1]=0.0;   Mat[2]=0.173648;
    Mat[4]=0.0;        Mat[5]=1.0;   Mat[6]=0.0;
    Mat[8]=-0.173648;   Mat[9]=0.0;   Mat[10]=0.984808;
    MatCamera[0]=MatCamera2[0]* Mat[0] + MatCamera2[1]* Mat[4] + MatCamera2[2]* Mat[8];
    MatCamera[1]=MatCamera2[0]* Mat[1] + MatCamera2[1]* Mat[5] + MatCamera2[2]* Mat[9];
    MatCamera[2]=MatCamera2[0]* Mat[2] + MatCamera2[1]* Mat[6] + MatCamera2[2]* Mat[10];
    MatCamera[4]=MatCamera2[4]* Mat[0] + MatCamera2[5]* Mat[4] + MatCamera2[6]* Mat[8];
    MatCamera[5]=MatCamera2[4]* Mat[1] + MatCamera2[5]* Mat[5] + MatCamera2[6]* Mat[9];
    MatCamera[6]=MatCamera2[4]* Mat[2] + MatCamera2[5]* Mat[6] + MatCamera2[6]* Mat[10];
    MatCamera[8]=MatCamera2[8]* Mat[0] + MatCamera2[9]* Mat[4] + MatCamera2[10]* Mat[8];
    MatCamera[9]=MatCamera2[8]* Mat[1] + MatCamera2[9]* Mat[5] + MatCamera2[10]* Mat[9];
    MatCamera[10]=MatCamera2[8]* Mat[2] + MatCamera2[9]* Mat[6] + MatCamera2[10]* Mat[10];
    break;
  case '2':
    Mat[0]=1.0;   Mat[1]=0.0;         Mat[2]=0.0;
    Mat[4]=0.0;   Mat[5]=0.984808;    Mat[6]=0.173648;
    Mat[8]=0.0;   Mat[9]=-0.173648;   Mat[10]=0.984808;
    MatCamera[0]=MatCamera2[0]* Mat[0] + MatCamera2[1]* Mat[4] + MatCamera2[2]* Mat[8];
    MatCamera[1]=MatCamera2[0]* Mat[1] + MatCamera2[1]* Mat[5] + MatCamera2[2]* Mat[9];
    MatCamera[2]=MatCamera2[0]* Mat[2] + MatCamera2[1]* Mat[6] + MatCamera2[2]* Mat[10];
    MatCamera[4]=MatCamera2[4]* Mat[0] + MatCamera2[5]* Mat[4] + MatCamera2[6]* Mat[8];
    MatCamera[5]=MatCamera2[4]* Mat[1] + MatCamera2[5]* Mat[5] + MatCamera2[6]* Mat[9];
    MatCamera[6]=MatCamera2[4]* Mat[2] + MatCamera2[5]* Mat[6] + MatCamera2[6]* Mat[10];
    MatCamera[8]=MatCamera2[8]* Mat[0] + MatCamera2[9]* Mat[4] + MatCamera2[10]* Mat[8];
    MatCamera[9]=MatCamera2[8]* Mat[1] + MatCamera2[9]* Mat[5] + MatCamera2[10]* Mat[9];
    MatCamera[10]=MatCamera2[8]* Mat[2] + MatCamera2[9]* Mat[6] + MatCamera2[10]* Mat[10];
    break;
  case '8':
    Mat[0]=1.0;   Mat[1]=0.0;         Mat[2]=0.0;
    Mat[4]=0.0;   Mat[5]=0.984808;    Mat[6]=-0.173648;
    Mat[8]=0.0;   Mat[9]=0.173648;   Mat[10]=0.984808;
    MatCamera[0]=MatCamera2[0]* Mat[0] + MatCamera2[1]* Mat[4] + MatCamera2[2]* Mat[8];
    MatCamera[1]=MatCamera2[0]* Mat[1] + MatCamera2[1]* Mat[5] + MatCamera2[2]* Mat[9];
    MatCamera[2]=MatCamera2[0]* Mat[2] + MatCamera2[1]* Mat[6] + MatCamera2[2]* Mat[10];
    MatCamera[4]=MatCamera2[4]* Mat[0] + MatCamera2[5]* Mat[4] + MatCamera2[6]* Mat[8];
    MatCamera[5]=MatCamera2[4]* Mat[1] + MatCamera2[5]* Mat[5] + MatCamera2[6]* Mat[9];
    MatCamera[6]=MatCamera2[4]* Mat[2] + MatCamera2[5]* Mat[6] + MatCamera2[6]* Mat[10];
    MatCamera[8]=MatCamera2[8]* Mat[0] + MatCamera2[9]* Mat[4] + MatCamera2[10]* Mat[8];
    MatCamera[9]=MatCamera2[8]* Mat[1] + MatCamera2[9]* Mat[5] + MatCamera2[10]* Mat[9];
    MatCamera[10]=MatCamera2[8]* Mat[2] + MatCamera2[9]* Mat[6] + MatCamera2[10]* Mat[10];
    break;

  case 'a':
    Dist_Camera_Sat--;//Zoom-
    break;

  case 'q':
    Dist_Camera_Sat++;//Zoom+
    break;

  case 27: 
    exit(0);
    break;
  };
}

/*functions TSP*/

int validate_symbol_info(char* symbol_name, 
			 const TSP_sample_symbol_info_list_t* tsp_symbols) {
  int     i;
  int32_t retval;
  char*   searched_array_symbol;

  retval = 0;
  i = strlen(symbol_name);
  searched_array_symbol = (char*)malloc(i+2);
  strncpy(searched_array_symbol,symbol_name,i);
  searched_array_symbol[i] = '[';
  searched_array_symbol[i+1] = '\0';

  for (i=0; i< tsp_symbols->TSP_sample_symbol_info_list_t_len; ++i) {
    /* scalar symbol match */
    if (0==strcmp(tsp_symbols->TSP_sample_symbol_info_list_t_val[i].name,symbol_name)) {
      printf("Scalar symbol match <%s>\n",symbol_name);
      retval =1;
      break;
    }
    /* 
     * consider arrays 
     * symbols whose name is found in several symbol 
     * and found symbol == searched symbol + '['
     * "toto" is an array iff we found "toto["
     */
    if (NULL != strstr(tsp_symbols->TSP_sample_symbol_info_list_t_val[i].name,searched_array_symbol)) {
      ++retval;
    }
  }
  return retval;
}

void tspinit(int argc, char* argv[])
{
  int i,j;
  const TSP_answer_sample_t* tsp_info=NULL;
  TSP_sample_symbol_info_list_t  *tsp_symbol_list;
  int nb_scalar_symbol=0; 
  int symbol_dim=0;
  int forced_period=0;
  int nb_symbols = 0;
  int var_index=0;

  int *phase;
  int *period;
  char **name;

  FILE *f=NULL;

  /* read config_file */
  f = fopen(config_file,"r");
  if (!f) 
    {
      printf("don t found config file: %s\n\n",config_file);
      exit(0);
    }
  fscanf(f,"%d\n",&nb_symbols);
  phase = (int*)malloc(sizeof(int) * nb_symbols);
  period = (int*)malloc(sizeof(int) * nb_symbols);
  name = (char**) malloc(sizeof(char*) * nb_symbols);
  for(i=0;i<nb_symbols;i++)
    name[i] = (char*)malloc(sizeof(int) * 256);
  for(i=0;i<nb_symbols;i++)
    fscanf(f,"%s %d\n",name[i],&period[i]);
  fclose(f);
  printf("Asking %d symbols:\n",nb_symbols);
  for(i=0;i<nb_symbols;i++)
    printf("%s\t%dHz\n",name[i],(int)(100.0/(float)period[i]));
  printf("\n");
  

  tsp_symbol_list = &symbol_list;
  if (TSP_STATUS_OK!=TSP_consumer_init(&argc, &argv))
    {
      printf("tsp error init\n\n");
      if (phase) free(phase);
      if (period) free(period);
      for(i=0;i<nb_symbols;i++)
	{
	  if (name[i]) free(name[i]);
	}
      if (name) free(name);
      exit(0);
    }

 /* install SIGINT handler (POSIX way) */
 my_action.sa_handler = &my_sighandler;  
 sigfillset(&my_action.sa_mask);
 my_action.sa_flags = SA_RESTART;
 sigaction(SIGINT,&my_action,&old_action);    

  /* 
   * Connect to the provider.
   */
  if (NULL == myproviders) {
    myproviders = (void**)calloc(1,sizeof(TSP_provider_t));
    myproviders[0] = TSP_consumer_connect_url(tsp_provider_url);
    /* Verify if there is at least one provider */
    if (0==myproviders[0]) {
      printf("no provider found: %s\n\n",tsp_provider_url);
      if (phase) free(phase);
      if (period) free(period);
      for(i=0;i<nb_symbols;i++)
	{
	  if (name[i]) free(name[i]);
	}
      if (name) free(name);
      exit(0);
    } else if (TSP_STATUS_OK!=TSP_consumer_request_open(myproviders[0], 0, 0 )) {
      printf("Cannot connect to provider <%s> (TSP_request_open failed.)",TSP_consumer_get_connected_name(myproviders[0]));
      if (phase) free(phase);
      if (period) free(period);
      for(i=0;i<nb_symbols;i++)
	{
	  if (name[i]) free(name[i]);
	}
      if (name) free(name);
      exit(0);
    }
  }
  /* send request info for getting symbols list */
  if(TSP_STATUS_OK!=TSP_consumer_request_information(myproviders[0])) {
    printf("TSP_request_information failed");
    if (phase) free(phase);
    if (period) free(period);
    for(i=0;i<nb_symbols;i++)
      {
	if (name[i]) free(name[i]);
      }
    if (name) free(name);
    exit(0);
  }
  
  tsp_info = TSP_consumer_get_information(myproviders[0]);	
  /* We now validate symbols from config file against
   * the list provided by the TSP provider
   * (we discover array var too)
   */
    nb_scalar_symbol = 0;
    forced_period    = -1;
    for (i=0;i<nb_symbols;++i) {
      symbol_dim = validate_symbol_info(name[i],&(tsp_info->symbols));
      /* symbol not found */
      if (0==symbol_dim) {
	printf("Symbol <%s> not found on provider side.\n",name[i]);
	/* hack for ignoring unfound symbols */
	phase[i]  = -1;
      } else { /* symbol found */
	printf("Asking for symbol <%s> with period <%d>\n",
		name[i],
		period[i]);
	/* 
	 * FIXME force period to be the same 
	 * as the first valid symbol
	 */
	if (-1 == forced_period) {
	  forced_period = period[i];
	} else {
	  if (period[i] != forced_period) {
	    period[i] = forced_period;
	    printf("[period forced to <%d>]",period[i]);
	  }
	}
	if (symbol_dim>1) {
	  printf(" [array of size <%d>]\n",symbol_dim);
	} else {
	  printf("\n");
	}
	/* 
	 * It's not so nice 
	 * but we use the phase to store symbol dim
	 * FIXME waiting for tsp to handle arrays !!!
	 */
	phase[i] = symbol_dim;
	nb_scalar_symbol     += symbol_dim;
      } /* else symbol found */
    } /* loop over symbols coming from config file */
 
  
  /* Now build request sample */
    tsp_symbol_list->TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*)calloc(nb_scalar_symbol, sizeof(TSP_sample_symbol_info_t));
    tsp_symbol_list->TSP_sample_symbol_info_list_t_len = nb_scalar_symbol;
    var_index = 0;
    for (i=0;i<nb_symbols; ++i) {
      /* 
       * Generate symbol name for array var specified without index.
       */
      if (phase[i] > 1) {
	for (j=0;j<phase[i];++j) {	    
	  tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].name = (char*)malloc(MAX_VAR_NAME_SIZE*sizeof(char));
	  snprintf(tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].name,
		   MAX_VAR_NAME_SIZE,		     
		   "%s[%0d]",
		   name[i],
		   j);
	  printf("Asking for TSP var = <%s>\n",tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].name);
	  tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].period = period[i];
	  tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].phase  = 0;
	  ++var_index;
	} /* loop over array var index */
      } else {
	/* ignore symbols with negative phase */
	if (phase[i] >0) {
	  tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].name   = strdup(name[i]);
	  printf("Asking for TSP var = <%s>\n",tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].name);
	  tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].period = period[i];
	  tsp_symbol_list->TSP_sample_symbol_info_list_t_val[var_index].phase  = 0;
	  ++var_index;
	}
      } /* end if phase[i] > 1 */
    } /* loop over nb_symbols */
  
  
  /* Now send request sample */
    if (TSP_STATUS_OK!=TSP_consumer_request_sample(myproviders[0],tsp_symbol_list)) {
      printf("TSP request sample refused by the provider?...\n");
      if (phase) free(phase);
      if (period) free(period);
      for(i=0;i<nb_symbols;i++)
	{
	  if (name[i]) free(name[i]);
	}
      if (name) free(name);
      exit(0);
    }

    /* Get previously configured symbols */
    symbols = TSP_consumer_get_requested_sample(myproviders[0]);

    if(TSP_STATUS_OK!=TSP_consumer_request_sample_init(myproviders[0],0,0)) {
      printf("Sample init refused by the provider??...\n\n");
      if (phase) free(phase);
      if (period) free(period);
      for(i=0;i<nb_symbols;i++)
	{
	  if (name[i]) free(name[i]);
	}
      if (name) free(name);
      exit(0);
    }
}

/*drawint text in OpenGL with GLUT*/
void DrawText(int x, int y, char *texte, int R, int G ,int B)
{
  int i;
  glDisable( GL_LIGHTING );  /* Disable lighting while we render text */
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0.0, 100.0, 0.0, 100.0  );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glColor3ub( R, G, B );
  glRasterPos2i( x, y );
  
  /*** Render the live character array 'text' ***/
  for( i=0; i<(int)strlen( texte); i++ )
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, texte[i] );
  glEnable( GL_LIGHTING );
  
}

/* Drawing 3ds ; this function is called by GLUT; just before draw , we read the last TSP info */
void myGlutDisplay()
{
  int i,stop_it = 1;
  char texte[100];
  float currinfo[NBINFO];
  float _time;

  for(i=0;i<NBINFO;i++)
    currinfo[i]=lastinfo[i];
  

  /*get the last info to draw*/
  /*cause the server may send more than one line during the last 3d draw*/
  /* write loop */
  while(stop_it && (TSP_STATUS_OK==TSP_consumer_read_sample(myproviders[0],&sample, &new_sample))) {
      if (new_sample) {
	//printf("%16.9E ",sample.user_value);
	lastinfo[complete_line]=sample.uvalue.double_value;
	++complete_line;
	/* We write the endl if we receive a whole sample line */
	if (symbols->TSP_sample_symbol_info_list_t_len==complete_line) {
	  //printf("\n");
	  complete_line = 0;
	  //printf("lecture ligne %f\n",lastinfo[4]);
	  for(i=0;i<NBINFO;i++)
	    currinfo[i]=lastinfo[i];
	}
      } else {
	stop_it=0;
	//printf("rien à lire, on a les dernieres infos envoyées\n");
      }
    } /* end of while ecriture */

    /*prepare info to draw*/
    Quaternion_Sat[0]=currinfo[0];Quaternion_Sat[1]=currinfo[1];Quaternion_Sat[2]=currinfo[2];Quaternion_Sat[3]=currinfo[3];
    Position_Sat[0]=currinfo[4]/1000.0;
    Position_Sat[1]=currinfo[5]/1000.0;
    Position_Sat[2]=currinfo[6]/1000.0;
    if (currinfo[7]<0.5) eclipsesun=1; else eclipsesun=0;
    if (currinfo[8]<0.5) eclipsemoon=1; else eclipsemoon=0;
    Position_Sun[0]=DIST_EARTH_SUN*currinfo[9];
    Position_Sun[1]=DIST_EARTH_SUN*currinfo[10];
    Position_Sun[2]=DIST_EARTH_SUN*currinfo[11];
    Position_Moon[0]=DIST_EARTH_MOON*currinfo[12];
    Position_Moon[1]=DIST_EARTH_MOON*currinfo[13];
    Position_Moon[2]=DIST_EARTH_MOON*currinfo[14];
    _time=currinfo[15];

    Position_Camera[0] = Position_Sat[0] - Dist_Camera_Sat *  MatCamera[8]; 
    Position_Camera[1] = Position_Sat[1] - Dist_Camera_Sat *  MatCamera[9]; 
    Position_Camera[2] = Position_Sat[2] - Dist_Camera_Sat *  MatCamera[10];

    glSceneSetCamera(&MyScene, Position_Sat[0], Position_Sat[1], Position_Sat[2], Dist_Camera_Sat, MatCamera);
    
    glSceneSetPosition3DS(&MyScene, n3ds_earth, Position_Earth[0], Position_Earth[1], Position_Earth[2]);
    glSceneSetPosition3DS(&MyScene, n3ds_sun, Position_Sun[0]/10000.0, Position_Sun[1]/10000.0, Position_Sun[2]/10000.0);
    glSceneSetPosition3DS(&MyScene, n3ds_moon, Position_Moon[0]/38.4, Position_Moon[1]/38.4, Position_Moon[2]/38.4);
    glSceneSetPosition3DS(&MyScene, n3ds_sat, Position_Sat[0], Position_Sat[1], Position_Sat[2]);
    glSceneSetPosition3DS(&MyScene, n3ds_pan, Position_Sat[0], Position_Sat[1], Position_Sat[2]);

    glSceneSetRotationQuaternion3DS(&MyScene, n3ds_sat, Quaternion_Sat[0], Quaternion_Sat[1], Quaternion_Sat[2], Quaternion_Sat[3]);
    glSceneSetRotationQuaternion3DS(&MyScene, n3ds_pan, Quaternion_Sat[0], Quaternion_Sat[1], Quaternion_Sat[2], Quaternion_Sat[3]);

    glSceneGlobalLightSetPosition(&MyScene, light_sun, Position_Sun[0], Position_Sun[1], Position_Sun[2]);
    glSceneGlobalLightSetPosition(&MyScene, light_ambient, Position_Camera[0], Position_Camera[1], Position_Camera[2]);
    if (eclipsesun)
      glSceneGlobalLightSetDiffuse(&MyScene, light_sun, 0.0, 0.0, 0.0, 1.0);
    else
      glSceneGlobalLightSetDiffuse(&MyScene, light_sun, 1.0, 1.0, 1.0, 1.0);
    glSceneMonoLightSetPosition(&MyScene, mono_light_sun_shining, Position_Camera[0], Position_Camera[1], Position_Camera[2]);


    /* draw the 3D info*/
    glSceneDraw(&MyScene);
    /* end draw the 3D info*/

    /*draw text info*/
    sprintf(texte,"Alt  Sat %.2f km",RADIUS_EARTH - sqrt(Position_Sat[0]*Position_Sat[0]+\
							 Position_Sat[1]*Position_Sat[1]+Position_Sat[2]*Position_Sat[2]));
    DrawText(2, 25, texte , 0, 255, 0);
    sprintf(texte,"Quat Sat %.2f %.2f %.2f %.2f ",Quaternion_Sat[0], Quaternion_Sat[1], Quaternion_Sat[2], Quaternion_Sat[3]);
    DrawText(2, 22, texte , 0, 255, 0);
    sprintf(texte,"Pos Sat  %.1f %.1f %.1f",Position_Sat[0], Position_Sat[1], Position_Sat[2]);
    DrawText(2, 19, texte , 0, 255, 0);
    sprintf(texte,"Pos Sun  %.1f %.1f %.1f (e+6 Km)",Position_Sun[0]/1000000.0, Position_Sun[1]/1000000.0, Position_Sun[2]/1000000.0);
    DrawText(2, 16, texte , 0, 255, 0);
    sprintf(texte,"Pos Moon  %.1f %.1f %.1f (e+3 Km)",Position_Moon[0]/1000.0, Position_Moon[1]/1000.0, Position_Moon[2]/1000.0);
    DrawText(2, 13, texte , 0, 255, 0);
    if (eclipsesun)
      sprintf(texte,"Sun  Eclipse: yes"); else sprintf(texte,"Sun  Eclipse: no");
    DrawText(2, 10, texte , 0, 255, 0);
    if (eclipsemoon)
      sprintf(texte,"Moon Eclipse: yes"); else sprintf(texte,"Moon Eclipse: no");
    DrawText(2, 7, texte , 0, 255, 0);
    sprintf(texte,"Time: %.1f",_time);
    DrawText(2, 4, texte , 0, 255, 0);
    DrawText(80, 7, "ZOOM:     a,q" , 255, 0, 0);
    DrawText(80, 4, "ROTATION: 2,4,6,8" , 255, 0, 0);
    /*end draw text info*/
  

    /* Swap the buffer*/
    glutSwapBuffers(); 
}

/*GLUT will call the display function every time*/
void myGlutIdle()
{
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  
  glutPostRedisplay();
}

int main(int argc, char* argv[])
{int i;

  if (argc!=3) {
    printf("Usage: %s <TSP provider URL> <sample_config_file>\n", argv[0]);
    printf("   the file specifying the list of symbols to be sampled\n");
    printf("   the  TSP provider URL <PROTOCOL://HOST/SERVER:PORT> \n");  
    printf("   example: TspVisu3DSat Config_TspVisu3DSat.dat \"rpc://10.220.133.35/:0\" \n");  
    exit(0);
  }

  strcpy(config_file     ,argv[1]);
  strcpy(tsp_provider_url,argv[2]);

  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 5, 5 );
  glutInitWindowSize( 800, 600 );
  main_window = glutCreateWindow( "VISU3D v0.02 Beta" );
  glutDisplayFunc( myGlutDisplay );
  glutKeyboardFunc( myGlutKeyboard );
  glutIdleFunc( myGlutIdle );


  glSceneInit(&MyScene); 
  printf("Scene 3DS: Loading Earth...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "earth.3DS",&n3ds_earth,RADIUS_EARTH);
  printf("Scene 3DS: Loading Moon...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "moon.3DS",&n3ds_moon,RADIUS_MOON);
  printf("Scene 3DS: Loading Sun...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "sun.3DS",&n3ds_sun,RADIUS_SUN);
  printf("Scene 3DS: Loading Satellite...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "moon.3DS",&n3ds_sat,RADIUS_SAT);
  printf("Scene 3DS: Loading Panels Satellite\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "Panneaux.3DS",&n3ds_pan,RADIUS_PAN); 
  printf("Scene 3DS: Loading Lights...\n");
  glSceneAddMonoLight(&MyScene, n3ds_sun ,&mono_light_sun_shining);
  glSceneAddGlobalLight(&MyScene, &light_sun);
  glSceneAddGlobalLight(&MyScene, &light_ambient);
  glSceneGlobalLightSetDiffuse(&MyScene, light_ambient, 0.1, 0.1, 0.1, 1.0);

  printf("Scene 3DS OK\n");

  /*TSP************************************************************/
  tspinit(argc, argv);

  for(i=0;i<NBINFO;i++)
    lastinfo[i]=0.0;
  lastinfo[0]=1.0;

  glutMainLoop();

  /*end tsp*/
  printf("Ending Visu3D...");
  TSP_consumer_request_sample_destroy(myproviders[0]);
  TSP_consumer_end();
  printf("OK\n");

  return 1;
}













