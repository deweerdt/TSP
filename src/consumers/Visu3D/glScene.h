/*!  \file 

$Id: glScene.h,v 1.1 2005-03-29 22:42:21 erk Exp $

-----------------------------------------------------------------------

glScene.h

Copyright (c) 2004.

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
Maintainer: olivier.monaret
Component : Visu3D :: OpenGL 3DS Tool

-----------------------------------------------------------------------
*/



#ifndef __GLSCENE_H__
#define __GLSCENE_H__


#define GLSCENE_MAX3DS 20

//today, the opengl support only 8 lights. Rarely more; this depends of graphical 3D cards.
#define GLSCENE_MAXGLOBALLIGHTS 8 
#define GLSCENE_MAXMONOLIGHTS 8   
#define GLSCENE_MAXTOTALLIGHTS 8   

#define GLSCENE_MAXTEXTUREPEROBJECT 20
#define GLSCENE_MAXTEXT 5 //max nb Text to draw

#include "Loader3ds.h"
#include <GL/gl.h>
#include <GL/glu.h>

/**************************************************************************************************************************************************/
// This is the structure of our Opengl Scene
/**************************************************************************************************************************************************/

typedef struct LightStruct {
  float Position[3]; //=  {0.0f, 0.0f, 0.0f};
  float Ambient[4];  //=  {0.1f, 0.1f, 0.1f, 1.0f};
  float Diffuse[4];  //=  {1.0f, 1.0f, 1.0f, 1.0f};
  float Specular[4]; //=  {0.3f, 0.3f, 0.3f, 1.0f};
} LightStruct;

typedef struct glSceneStruct {

  //3DS
  Loader3ds Obj3DS[GLSCENE_MAX3DS];
  float Obj3DSPosition[GLSCENE_MAX3DS][3];
  float Obj3DSRotation[GLSCENE_MAX3DS][16];
  float Obj3DSScale[GLSCENE_MAX3DS][3];
  unsigned short State3DS[GLSCENE_MAX3DS];   //0==empty   1==loaded_but_not_to_draw    2==loaded_and_to_draw
  unsigned short Obj3DSUseGlobalLights[GLSCENE_MAX3DS]; //default = 1
  unsigned int gTexture[GLSCENE_MAX3DS][GLSCENE_MAXTEXTUREPEROBJECT];
  unsigned short DrawAxis;//default 1;
  int firsttime3DS[GLSCENE_MAX3DS];

  //Camera
  float CameraPosition[3];
  float CameraRotation[16];

  //Global Lights(lights for all the 3ds objects of the scene)
  LightStruct GlobalLights[GLSCENE_MAXGLOBALLIGHTS];
  unsigned short StateGlobalLight[GLSCENE_MAXGLOBALLIGHTS]; //0==empty   1==loaded_but_not_to_use    2==loaded_and_to_use

  //Mono Lights (lights for only one 3ds object of the scene)
  LightStruct MonoLights[GLSCENE_MAXMONOLIGHTS];
  unsigned short StateMonoLight[GLSCENE_MAXMONOLIGHTS]; //0==empty   1==loaded_but_not_to_use    2==loaded_and_to_use
  int MonoLightForN3DS[GLSCENE_MAXMONOLIGHTS]; // the number of the 3ds object which use the light

  //Text
  char Text[GLSCENE_MAXTEXT][100];
  int TextColor[GLSCENE_MAXTEXT][3];
  int TextPos[GLSCENE_MAXTEXT][2];
  unsigned short StateText[GLSCENE_MAXTEXT]; //0==empty   1==loaded_but_not_to_use    2==loaded_and_to_use
  
} glSceneStruct;





typedef struct {
  const GLsizei width;
  const GLsizei height;
  const GLfloat xorig;
  const GLfloat yorig;
  const GLfloat advance;
  const GLubyte *bitmap;
} glSceneBitmapCharRec, *glSceneBitmapCharPtr;

typedef struct {
  const char *name;
  const int num_chars;
  const int first;
  const glSceneBitmapCharRec * const *ch;
} glSceneBitmapFontRec, *glSceneBitmapFontPtr;

//typedef void *glScenebitmapFont;




/**************************************************************************************************************************************************/
// End structure Opengl Scene
/**************************************************************************************************************************************************/



/**************************************************************************************************************************************************/
// API Functions: all the functions to make a 3D OpenGL Scene are here:
/**************************************************************************************************************************************************/

void glSceneInit(glSceneStruct *Scene); 
void glSceneCreate(glSceneStruct *Scene);
void glSceneDestroy(glSceneStruct *Scene); 

//Load a 3ds object: 
//Load with radius to adjust the scale
void glSceneAdd3DS(glSceneStruct *Scene, char* directory, char* filename3DS, int *N3DS);    // Load 3DS and Set State3DS[]=2 (default)
void glSceneAdd3DSRadius(glSceneStruct *Scene, char* directory, char* filename3DS, int *N3DS, float Radius);    // Load 3DS and Set State3DS[]=2 (default)   Radius adjust the Scale
void glSceneDelete3DS(glSceneStruct *Scene, int N3DS);
void glSceneEnable3DS(glSceneStruct *Scene, int N3DS);                     // Set State3DS[]=2 
void glSceneDisable3DS(glSceneStruct *Scene, int N3DS);                    // Set State3DS[]=1
void glSceneSetPosition3DS(glSceneStruct *Scene, int N3DS, float x, float y, float z);
void glSceneSetRotationQuaternion3DS(glSceneStruct *Scene, int N3DS, float q0, float q1, float q2, float q3);
void glSceneSetRotationMatrix3DS(glSceneStruct *Scene, int N3DS, float *Mat); // Mat is a 4x4 Matrix (use array[16])

//DrawAxis
void glSceneEnableAxis(glSceneStruct *Scene);
void glSceneDisableAxis(glSceneStruct *Scene);

void glSceneSetPositionCamera(glSceneStruct *Scene, float x, float y, float z);
void glSceneSetRotationQuaternionCamera(glSceneStruct *Scene, float q0, float q1, float q2, float q3);
void glSceneSetRotationMatrixCamera(glSceneStruct *Scene, float *Mat); // Mat is a 4x4 Matrix (use array[16])
void glSceneSetCamera(glSceneStruct *Scene, float lookatx, float lookaty, float lookatz, float dist, float *Mat);// Mat is a 4x4 Matrix (use array[16])

//GLOBAL Light is used for all objects 
void glSceneAddGlobalLight(glSceneStruct *Scene, int *NGL);    // Set StateGlobalLight[]=2 (default)
void glSceneDeleteGlobalLight(glSceneStruct *Scene, int NGL);
void glSceneEnableGlobalLight(glSceneStruct *Scene, int NGL);                     // Set StateGlobalLight[]=2 
void glSceneDisableGlobalLight(glSceneStruct *Scene, int NGL);                    // Set StateGlobalLight[]=1
void glSceneGlobalLightSetPosition(glSceneStruct *Scene, int NGL, float x, float y, float z);
void glSceneGlobalLightSetAmbient( glSceneStruct *Scene, int NGL, float a, float b, float c, float d);
void glSceneGlobalLightSetDiffuse(glSceneStruct *Scene, int NGL, float a, float b, float c, float d);
void glSceneGlobalLightSetSpecular(glSceneStruct *Scene, int NGL, float a, float b, float c, float d);

//MONO Light is used only for one object
void glSceneAddMonoLight(glSceneStruct *Scene, int Number3DSObject, int *NML);    // Set StateMonoLight[]=2 (default)
void glSceneDeleteMonoLight(glSceneStruct *Scene, int NML);
void glSceneEnableMonoLight(glSceneStruct *Scene, int NML);                     // Set StateMonoLight[]=2 
void glSceneDisableMonoLight(glSceneStruct *Scene, int NML);                    // Set StateMonoLight[]=1
void glSceneMonoLightSetPosition(glSceneStruct *Scene, int NML, float x, float y, float z);
void glSceneMonoLightSetAmbient( glSceneStruct *Scene, int NML, float a, float b, float c, float d);
void glSceneMonoLightSetDiffuse(glSceneStruct *Scene, int NML, float a, float b, float c, float d);
void glSceneMonoLightSetSpecular(glSceneStruct *Scene, int NML, float a, float b, float c, float d);

     ///////////////////////////////////////////////////////////////////////////////////////////////////////
void glSceneDraw(glSceneStruct *Scene);
     //AFTER CALLING THIS FUNCTION YOU MUST SWAP THE BUFFER OF THE WINDOW TO DRAW THE OPENGL AREA
     //////////////////////////////////////////////////////////////////////////////////////////////////////

/*TODO*/
/*TODO*/void glSceneAddText(glSceneStruct *Scene, int x/*[0;100]*/, int y/*[0;100]*/,char* str, int R, int G, int B, int *NT);
/*TODO*/void glSceneDeleteText(glSceneStruct *Scene, int NT);
/*TODO*/void glSceneEnableText(glSceneStruct *Scene, int NT);
/*TODO*/void glSceneDisableText(glSceneStruct *Scene, int NT);
/*TODO*/void glSceneSetText(glSceneStruct *Scene, int NT, char* str);
/*TODO*/void glSceneSetTextPosition(glSceneStruct *Scene, int NT, int x/*[0;100]*/, int y/*[0;100]*/, char* str);


/**************************************************************************************************************************************************/
// End API Functions
/**************************************************************************************************************************************************/


/**************************************************************************************************************************************************/
// Private Functions: don't call these functions
/**************************************************************************************************************************************************/
void glSceneLoadTextures(Loader3ds *Obj3DS, char* DirectoryTexture, unsigned int *gTexture);
void glSceneEnableLight(int light,float *Pos,float *Amb,float *Dif,float *Spe);
unsigned int glScenegetshort(FILE *fp);
unsigned int glScenegetint(FILE *fp);
unsigned int glSceneCreateTextureBMP(char *fileBMP);
unsigned int glSceneCreateTextureTGA(char *fileTGA);
void glSceneDraw3DSCallList(Loader3ds *My3DS, unsigned int *gTexture, GLuint VALUE_CALLLIST, int firsttime);
void glSceneDraw3DS(Loader3ds *My3DS, unsigned int *gTexture);
void glSceneDrawAxis( float l );
void glSceneDrawBase();
void glSceneBitmapCharacter(glSceneBitmapFontPtr font, int c);
void glSceneDrawText(int x/*[0;100]*/, int y/*[0;100]*/,char* str, int R, int G, int B);

/**************************************************************************************************************************************************/
// End Private Functions
/**************************************************************************************************************************************************/

#endif /* __GLSCENE_H__ */
