
/*!  \file 

$Id: test.c,v 1.1 2005-03-29 22:42:21 erk Exp $

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

December 2004
****************************************************************************/

#include <stdio.h>
#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>

#include "glScene.h"//Api OpenGL ; it uses GL.H and GLU.H


#define MAX_VAR_NAME_SIZE 256
#define NBINFO 16


/* Constants for 3ds space*/
#define RADIUS_EARTH 637.8
#define RADIUS_SAT   3.0
#define RADIUS_SUN   2.0 // real: 696000 km 
#define RADIUS_MOON  40.0 // real: 1700 km  
#define DIST_EARTH_MOON 3840.0 // real: 384000 km
#define DIST_EARTH_SUN  1500000.0 // real: 150.000.000 km
/**************************/

/**********************************************************************/
/* Variables 3d*/

glSceneStruct MyScene;

int n3ds_earth;
int n3ds_sat;
int n3ds_moon;
int n3ds_sun;
int light_sun;

float Position_Earth[]={0.0,0.0,0.0}; 
float Position_Sat[]={800.0,0.0,200.0}; 
float Position_Moon[]={1200.0,800.0,-2000.0}; 
float Position_Sun[]={2000.0,0.0,0.0}; 
float Quaternion_Sat[]={1.0,0.0,0.0,0.0}; 
float Dist_Camera_Sat=3.0;

float MatCamera[]={-1,0,0,0, 0,1,0,0, 0,0,-1,0, 0,0,0,1}; 

int   main_window;


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

/* Drawing 3ds ; this function is called by GLUT; just before draw , we read the last TSP info */
void myGlutDisplay()
{

    glSceneSetCamera(&MyScene, Position_Sat[0], Position_Sat[1], Position_Sat[2], Dist_Camera_Sat, MatCamera);
    
    glSceneSetPosition3DS(&MyScene, n3ds_earth, Position_Earth[0], Position_Earth[1], Position_Earth[2]);
    glSceneSetPosition3DS(&MyScene, n3ds_sat, Position_Sat[0], Position_Sat[1], Position_Sat[2]);
    glSceneSetPosition3DS(&MyScene, n3ds_sun, Position_Sun[0], Position_Sun[1], Position_Sun[2]);
	glSceneSetPosition3DS(&MyScene, n3ds_moon, Position_Moon[0], Position_Moon[1], Position_Moon[2]);

	glSceneGlobalLightSetDiffuse(&MyScene, light_sun, 1.0, 1.0, 0.9, 0.0);
	glSceneGlobalLightSetPosition(&MyScene, light_sun, 1000.0, 0.0, 0.0);

    /* draw the 3D info*/
    glSceneDraw(&MyScene);
    /* end draw the 3D info*/  

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
{

  //GLUT INIT
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 5, 5 );
  glutInitWindowSize( 800, 600 );
  main_window = glutCreateWindow( "VISU3D v0.02 Beta" );
  glutDisplayFunc( myGlutDisplay );
  glutKeyboardFunc( myGlutKeyboard );
  glutIdleFunc( myGlutIdle );

  //GLSCENE LOADING
  glSceneInit(&MyScene); 
  printf("Scene 3DS: Loading Earth...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "earth.3DS",&n3ds_earth,RADIUS_EARTH);
  printf("Scene 3DS: Loading Satellite...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "sat.3DS",&n3ds_sat,RADIUS_SAT);
  printf("Scene 3DS: Loading Moon...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "moon.3DS",&n3ds_moon,RADIUS_MOON);
  printf("Scene 3DS: Loading Sun...\n");
  glSceneAdd3DSRadius(&MyScene, "Data/", "sun.3DS",&n3ds_sun,RADIUS_SUN);
  printf("Scene 3DS: Loading Lights...\n");
  glSceneAddGlobalLight(&MyScene, &light_sun);
 
  printf("Scene 3DS OK\n");

  //GLUTLOOP
  glutMainLoop();

 
  return 1;
}













