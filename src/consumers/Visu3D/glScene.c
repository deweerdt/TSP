/*!  \file 

$Id: glScene.c,v 1.1 2005-03-29 22:42:21 erk Exp $

-----------------------------------------------------------------------

glScene.cpp

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

Project   : VISU3D
Maintainer: olivier.monaret
Component : OpenGL 3DS Tool

-----------------------------------------------------------------------
*/

#include "glScene.h"


void glSceneInit(glSceneStruct *Scene)
{
  int i;
  Scene->CameraPosition[0]=0.0; Scene->CameraPosition[1]=0.0; Scene->CameraPosition[2]=0.0;
  Scene->CameraRotation[0]=1.0; Scene->CameraRotation[1]=0.0; Scene->CameraRotation[2]=0.0; Scene->CameraRotation[3]=0.0; 
  Scene->CameraRotation[4]=0.0; Scene->CameraRotation[5]=1.0; Scene->CameraRotation[6]=0.0; Scene->CameraRotation[7]=0.0; 
  Scene->CameraRotation[8]=0.0; Scene->CameraRotation[9]=0.0; Scene->CameraRotation[10]=1.0; Scene->CameraRotation[11]=0.0; 
  Scene->CameraRotation[12]=0.0; Scene->CameraRotation[13]=0.0; Scene->CameraRotation[14]=0.0; Scene->CameraRotation[15]=1.0; 

  for(i=0;i<GLSCENE_MAX3DS;i++)
    {
      ReBoot(&Scene->Obj3DS[i]);
      Scene->State3DS[i] = 0;
      Scene->Obj3DSUseGlobalLights[i]=1;
      Scene->Obj3DSPosition[i][0]=0.0; Scene->Obj3DSPosition[i][1]=0.0; Scene->Obj3DSPosition[i][2]=0.0;
      Scene->Obj3DSRotation[i][0]=1.0; Scene->Obj3DSRotation[i][1]=0.0; Scene->Obj3DSRotation[i][2]=0.0; Scene->Obj3DSRotation[i][3]=0.0; 
      Scene->Obj3DSRotation[i][4]=0.0; Scene->Obj3DSRotation[i][5]=1.0; Scene->Obj3DSRotation[i][6]=0.0; Scene->Obj3DSRotation[i][7]=0.0; 
      Scene->Obj3DSRotation[i][8]=0.0; Scene->Obj3DSRotation[i][9]=0.0; Scene->Obj3DSRotation[i][10]=1.0; Scene->Obj3DSRotation[i][11]=0.0; 
      Scene->Obj3DSRotation[i][12]=0.0; Scene->Obj3DSRotation[i][13]=0.0; Scene->Obj3DSRotation[i][14]=0.0; Scene->Obj3DSRotation[i][15]=1.0; 
      Scene->Obj3DSScale[i][0]=1.0; Scene->Obj3DSScale[i][1]=1.0; Scene->Obj3DSScale[i][2]=1.0;
      Scene->firsttime3DS[i]=1;
    }

  for(i=0;i<GLSCENE_MAXGLOBALLIGHTS;i++)
    {
      Scene->State3DS[i] = 0;
      Scene->GlobalLights[i].Position[0] = 0.0; Scene->GlobalLights[i].Position[1] = 0.0; Scene->GlobalLights[i].Position[2] = 0.0;
      Scene->GlobalLights[i].Ambient[0]  = 0.1; Scene->GlobalLights[i].Ambient[1]  = 0.1; Scene->GlobalLights[i].Ambient[2]  = 0.1; Scene->GlobalLights[i].Ambient[3]  = 1.0;
      Scene->GlobalLights[i].Diffuse[0]  = 1.0; Scene->GlobalLights[i].Diffuse[1]  = 1.0; Scene->GlobalLights[i].Diffuse[2]  = 1.0; Scene->GlobalLights[i].Diffuse[3]  = 1.0;
      Scene->GlobalLights[i].Specular[0] = 0.3; Scene->GlobalLights[i].Specular[1] = 0.3; Scene->GlobalLights[i].Specular[2] = 0.3; Scene->GlobalLights[i].Specular[3] = 1.0;
    }

  for(i=0;i<GLSCENE_MAXMONOLIGHTS;i++)
    {
      Scene->State3DS[i] = 0;
      Scene->MonoLightForN3DS[i] = 0;
      Scene->MonoLights[i].Position[0] = 0.0; Scene->MonoLights[i].Position[1] = 0.0; Scene->MonoLights[i].Position[2] = 0.0; 
      Scene->MonoLights[i].Ambient[0]  = 0.1; Scene->MonoLights[i].Ambient[1]  = 0.1; Scene->MonoLights[i].Ambient[2]  = 0.1; Scene->MonoLights[i].Ambient[3]  = 1.0;
      Scene->MonoLights[i].Diffuse[0]  = 1.0; Scene->MonoLights[i].Diffuse[1]  = 1.0; Scene->MonoLights[i].Diffuse[2]  = 1.0; Scene->MonoLights[i].Diffuse[3]  = 1.0;
      Scene->MonoLights[i].Specular[0] = 0.3; Scene->MonoLights[i].Specular[1] = 0.3; Scene->MonoLights[i].Specular[2] = 0.3; Scene->MonoLights[i].Specular[3] = 1.0;
    }

  for(i=0;i<GLSCENE_MAXTEXT;i++)
    {
      Scene->StateText[i]=0;
      strcpy(Scene->Text[i],"");
      Scene->TextColor[i][0]=255;
      Scene->TextColor[i][1]=0;
      Scene->TextColor[i][2]=0;
      Scene->TextPos[i][0]=10;
      Scene->TextPos[i][1]=10;
    }


  Scene->DrawAxis=1;
}

void glSceneCreate(glSceneStruct *Scene)
{
  glSceneInit(Scene);
}

void glSceneDestroy(glSceneStruct *Scene)
{
  glSceneInit(Scene);
}

void glSceneAdd3DS(glSceneStruct *Scene, char* directory, char* filename3DS, int *N3DS)
{
  int i;
  char str[500];
  
  strcpy(str,directory);
  strcat(str,filename3DS);
  for(i=0;i<GLSCENE_MAX3DS;i++)
    if(Scene->State3DS[i]==0)
      {
	*N3DS=i;
	ReBoot(&Scene->Obj3DS[i]);
	Scene->State3DS[i] = 2;
	Import3DS(&Scene->Obj3DS[i],str);
	glSceneLoadTextures(&(Scene->Obj3DS[i]),directory,Scene->gTexture[i]);
	Scene->Obj3DSUseGlobalLights[i]=1;
	Scene->Obj3DSScale[i][0]=1.0; Scene->Obj3DSScale[i][1]=1.0; Scene->Obj3DSScale[i][2]=1.0;

	i=GLSCENE_MAX3DS;// Break the FOR
      }
}

void glSceneAdd3DSRadius(glSceneStruct *Scene, char* directory, char* filename3DS, int *N3DS, float Radius)    // Load 3DS and Set State3DS[]=2 (default)   Radius adjust the Scale
{
  int i;
  char str[500];
  float scale=1.0;
  
  strcpy(str,directory);
  strcat(str,filename3DS);
  for(i=0;i<GLSCENE_MAX3DS;i++)
    if(Scene->State3DS[i]==0)
      {
	*N3DS=i;
	ReBoot(&Scene->Obj3DS[i]);
	Scene->State3DS[i] = 2;
	Import3DS(&Scene->Obj3DS[i],str);
	glSceneLoadTextures(&(Scene->Obj3DS[i]),directory,Scene->gTexture[i]);
	Scene->Obj3DSUseGlobalLights[i]=1;
	scale= Radius  / Scene->Obj3DS[i].max;
	Scene->Obj3DSScale[i][0]=scale; Scene->Obj3DSScale[i][1]=scale; Scene->Obj3DSScale[i][2]=scale;

	i=GLSCENE_MAX3DS;// Break the FOR
      }
}


void glSceneDelete3DS(glSceneStruct *Scene, int N3DS)
{
  ReBoot(&Scene->Obj3DS[N3DS]);
  Scene->State3DS[N3DS] = 0;
  Scene->Obj3DSUseGlobalLights[N3DS]=1;
  Scene->Obj3DSScale[N3DS][0]=1.0; Scene->Obj3DSScale[N3DS][1]=1.0; Scene->Obj3DSScale[N3DS][2]=1.0;
  Scene->firsttime3DS[N3DS]=1;
}

void glSceneEnable3DS(glSceneStruct *Scene, int N3DS)                     // Set State3DS[]=2 
{
  if (Scene->State3DS[N3DS] != 0)
    Scene->State3DS[N3DS] = 2;
  Scene->firsttime3DS[N3DS]=1;
}

void glSceneDisable3DS(glSceneStruct *Scene, int N3DS)                    // Set State3DS[]=1
{
  if (Scene->State3DS[N3DS] != 0)
    Scene->State3DS[N3DS] = 1;
}

void glSceneSetPosition3DS(glSceneStruct *Scene, int N3DS, float x, float y, float z)
{
  Scene->Obj3DSPosition[N3DS][0]=x; Scene->Obj3DSPosition[N3DS][1]=y; Scene->Obj3DSPosition[N3DS][2]=z;
}

void glSceneSetRotationQuaternion3DS(glSceneStruct *Scene, int N3DS, float q0, float q1, float q2, float q3)
{
   Scene->Obj3DSRotation[N3DS][0]=1.0 - 2.0*q2*q2 - 2.0*q3*q3;	
   Scene->Obj3DSRotation[N3DS][1]=2.0*q1*q2 - 2.0*q3*q0;	
   Scene->Obj3DSRotation[N3DS][2]=2.0*q1*q3 + 2.0*q2*q0;	
   Scene->Obj3DSRotation[N3DS][3]=0.0;

   Scene->Obj3DSRotation[N3DS][4]=2.0*q1*q2 + 2.0*q3*q0;
   Scene->Obj3DSRotation[N3DS][5]=1.0 - 2.0*q1*q1 - 2.0*q3*q3;
   Scene->Obj3DSRotation[N3DS][6]=2.0*q2*q3 - 2.0*q1*q0;
   Scene->Obj3DSRotation[N3DS][7]=0.0;

   Scene->Obj3DSRotation[N3DS][8]=2.0*q1*q3 - 2.0*q2*q0;
   Scene->Obj3DSRotation[N3DS][9]=2.0*q2*q3 + 2.0*q1*q0;
   Scene->Obj3DSRotation[N3DS][10]=1.0 - 2.0*q1*q1 - 2.0*q2*q2;
   Scene->Obj3DSRotation[N3DS][11]=0.0;

   Scene->Obj3DSRotation[N3DS][12]=0.0;
   Scene->Obj3DSRotation[N3DS][13]=0.0;	
   Scene->Obj3DSRotation[N3DS][14]=0.0;	
   Scene->Obj3DSRotation[N3DS][15]=1.0;
}

void glSceneSetRotationMatrix3DS(glSceneStruct *Scene, int N3DS, float *Mat) // Mat is a 4x4 Matrix
{
  Scene->Obj3DSRotation[N3DS][0] = Mat[0]; Scene->Obj3DSRotation[N3DS][1] = Mat[1]; Scene->Obj3DSRotation[N3DS][2] = Mat[2]; 
  Scene->Obj3DSRotation[N3DS][3] = Mat[3]; Scene->Obj3DSRotation[N3DS][4] = Mat[4]; Scene->Obj3DSRotation[N3DS][5] = Mat[5]; 
  Scene->Obj3DSRotation[N3DS][6] = Mat[6]; Scene->Obj3DSRotation[N3DS][7] = Mat[7]; Scene->Obj3DSRotation[N3DS][8] = Mat[8]; 
  Scene->Obj3DSRotation[N3DS][9] = Mat[9]; Scene->Obj3DSRotation[N3DS][10] = Mat[10]; Scene->Obj3DSRotation[N3DS][11] = Mat[11]; 
  Scene->Obj3DSRotation[N3DS][12] = Mat[12]; Scene->Obj3DSRotation[N3DS][13] = Mat[13]; Scene->Obj3DSRotation[N3DS][14] = Mat[14]; 
  Scene->Obj3DSRotation[N3DS][15] = Mat[15]; 
}

void glSceneEnableAxis(glSceneStruct *Scene)
{
  Scene->DrawAxis=1;
}
void glSceneDisableAxis(glSceneStruct *Scene)
{
  Scene->DrawAxis=0;
}

void glSceneSetPositionCamera(glSceneStruct *Scene, float x, float y, float z)
{
  Scene->CameraPosition[0]=x; Scene->CameraPosition[1]=y; Scene->CameraPosition[2]=z;
}
void glSceneSetRotationQuaternionCamera(glSceneStruct *Scene, float q0, float q1, float q2, float q3)
{
   Scene->CameraRotation[0]=1.0 - 2.0*q2*q2 - 2.0*q3*q3;	
   Scene->CameraRotation[1]=2.0*q1*q2 - 2.0*q3*q0;	
   Scene->CameraRotation[2]=2.0*q1*q3 + 2.0*q2*q0;	
   Scene->CameraRotation[3]=0.0;

   Scene->CameraRotation[4]=2.0*q1*q2 + 2.0*q3*q0;
   Scene->CameraRotation[5]=1.0 - 2.0*q1*q1 - 2.0*q3*q3;
   Scene->CameraRotation[6]=2.0*q2*q3 - 2.0*q1*q0;
   Scene->CameraRotation[7]=0.0;

   Scene->CameraRotation[8]=2.0*q1*q3 - 2.0*q2*q0;
   Scene->CameraRotation[9]=2.0*q2*q3 + 2.0*q1*q0;
   Scene->CameraRotation[10]=1.0 - 2.0*q1*q1 - 2.0*q2*q2;
   Scene->CameraRotation[11]=0.0;

   Scene->CameraRotation[12]=0.0;
   Scene->CameraRotation[13]=0.0;	
   Scene->CameraRotation[14]=0.0;	
   Scene->CameraRotation[15]=1.0;
}
void glSceneSetRotationMatrixCamera(glSceneStruct *Scene, float *Mat) // Mat is a 4x4 Matrix
{
  Scene->CameraRotation[0] = Mat[0]; Scene->CameraRotation[1] = Mat[1]; Scene->CameraRotation[2] = Mat[2]; 
  Scene->CameraRotation[3] = Mat[3]; Scene->CameraRotation[4] = Mat[4]; Scene->CameraRotation[5] = Mat[5]; 
  Scene->CameraRotation[6] = Mat[6]; Scene->CameraRotation[7] = Mat[7]; Scene->CameraRotation[8] = Mat[8]; 
  Scene->CameraRotation[9] = Mat[9]; Scene->CameraRotation[10] = Mat[10]; Scene->CameraRotation[11] = Mat[11]; 
  Scene->CameraRotation[12] = Mat[12]; Scene->CameraRotation[13] = Mat[13]; Scene->CameraRotation[14] = Mat[14]; 
  Scene->CameraRotation[15] = Mat[15]; 
}

void glSceneSetCamera(glSceneStruct *Scene, float lookatx, float lookaty, float lookatz, float dist, float *Mat)// Mat is a 4x4 Matrix (use array[16])
{
  Scene->CameraPosition[0] = lookatx + dist *  Mat[2]; // i don t why, it's a bug. (10 euros to win)
  Scene->CameraPosition[1] = lookaty - dist *  Mat[6]; 
  Scene->CameraPosition[2] = lookatz - dist *  Mat[10];

  Scene->CameraRotation[0] = Mat[0]; Scene->CameraRotation[1] = Mat[1]; Scene->CameraRotation[2] = Mat[2]; 
  Scene->CameraRotation[3] = Mat[3]; Scene->CameraRotation[4] = Mat[4]; Scene->CameraRotation[5] = Mat[5]; 
  Scene->CameraRotation[6] = Mat[6]; Scene->CameraRotation[7] = Mat[7]; Scene->CameraRotation[8] = Mat[8]; 
  Scene->CameraRotation[9] = Mat[9]; Scene->CameraRotation[10] = Mat[10]; Scene->CameraRotation[11] = Mat[11]; 
  Scene->CameraRotation[12] = Mat[12]; Scene->CameraRotation[13] = Mat[13]; Scene->CameraRotation[14] = Mat[14]; 
  Scene->CameraRotation[15] = Mat[15]; 

}

/*** GLOBAL LIGHTS **********************************************************************************************************/

void glSceneAddGlobalLight(glSceneStruct *Scene, int *NGL)    // Set StateGlobalLight[]=2 (default)
{
  int i;
  for(i=0;i<GLSCENE_MAXGLOBALLIGHTS;i++)
    if(Scene->StateGlobalLight[i]==0)
      {
	*NGL=i;
	Scene->GlobalLights[i].Position[0] = 0.0; Scene->GlobalLights[i].Position[1] = 0.0; Scene->GlobalLights[i].Position[2] = 0.0;
	Scene->GlobalLights[i].Ambient[0]  = 0.1; Scene->GlobalLights[i].Ambient[1]  = 0.1; Scene->GlobalLights[i].Ambient[2]  = 0.1; Scene->GlobalLights[i].Ambient[3]  = 1.0;
	Scene->GlobalLights[i].Diffuse[0]  = 1.0; Scene->GlobalLights[i].Diffuse[1]  = 1.0; Scene->GlobalLights[i].Diffuse[2]  = 1.0; Scene->GlobalLights[i].Diffuse[3]  = 1.0;
	Scene->GlobalLights[i].Specular[0] = 0.3; Scene->GlobalLights[i].Specular[1] = 0.3; Scene->GlobalLights[i].Specular[2] = 0.3; Scene->GlobalLights[i].Specular[3] = 1.0;
	Scene->StateGlobalLight[i] = 2;

	i=GLSCENE_MAXGLOBALLIGHTS; // Break the FOR
      }
}

void glSceneDeleteGlobalLight(glSceneStruct *Scene, int NGL)
{
  Scene->StateGlobalLight[NGL] = 0;
}

void glSceneEnableGlobalLight(glSceneStruct *Scene, int NGL)                     // Set StateGlobalLight[]=2 
{
  if (Scene->StateGlobalLight[NGL] != 0)
    Scene->StateGlobalLight[NGL] = 2;
}

void glSceneDisableGlobalLight(glSceneStruct *Scene, int NGL)                    // Set StateGlobalLight[]=1
{
  if (Scene->StateGlobalLight[NGL] != 0)
    Scene->StateGlobalLight[NGL] = 1;
}

void glSceneGlobalLightSetPosition(glSceneStruct *Scene, int NGL, float x, float y, float z)
{
  Scene->GlobalLights[NGL].Position[0] = x; Scene->GlobalLights[NGL].Position[1] = y; Scene->GlobalLights[NGL].Position[2] = z; 
}

void glSceneGlobalLightSetAmbient( glSceneStruct *Scene, int NGL, float a, float b, float c, float d)
{
  Scene->GlobalLights[NGL].Ambient[0] = a; Scene->GlobalLights[NGL].Ambient[1] = b; Scene->GlobalLights[NGL].Ambient[2] = c;  Scene->GlobalLights[NGL].Ambient[3] = d;
}

void glSceneGlobalLightSetDiffuse(glSceneStruct *Scene, int NGL, float a, float b, float c, float d)
{
  Scene->GlobalLights[NGL].Diffuse[0] = a; Scene->GlobalLights[NGL].Diffuse[1] = b; Scene->GlobalLights[NGL].Diffuse[2] = c;  Scene->GlobalLights[NGL].Diffuse[3] = d;
}

void glSceneGlobalLightSetSpecular(glSceneStruct *Scene, int NGL, float a, float b, float c, float d)
{
  Scene->GlobalLights[NGL].Specular[0] = a; Scene->GlobalLights[NGL].Specular[1] = b; Scene->GlobalLights[NGL].Specular[2] = c;  Scene->GlobalLights[NGL].Specular[3] = d;
}

/*** MONO  LIGHTS **********************************************************************************************************/

void glSceneAddMonoLight(glSceneStruct *Scene, int Number3DSObject, int *NML)    // Set StateMonoLight[]=2 (default)
{
  int i;
  for(i=0;i<GLSCENE_MAXMONOLIGHTS;i++)
    if(Scene->StateMonoLight[i]==0)
      {
	*NML=i;
	Scene->MonoLights[i].Position[0] = 0.0; Scene->MonoLights[i].Position[1] = 0.0; Scene->MonoLights[i].Position[2] = 0.0;
	Scene->MonoLights[i].Ambient[0]  = 0.1; Scene->MonoLights[i].Ambient[1]  = 0.1; Scene->MonoLights[i].Ambient[2]  = 0.1; Scene->MonoLights[i].Ambient[3]  = 1.0;
	Scene->MonoLights[i].Diffuse[0]  = 1.0; Scene->MonoLights[i].Diffuse[1]  = 1.0; Scene->MonoLights[i].Diffuse[2]  = 1.0; Scene->MonoLights[i].Diffuse[3]  = 1.0;
	Scene->MonoLights[i].Specular[0] = 0.3; Scene->MonoLights[i].Specular[1] = 0.3; Scene->MonoLights[i].Specular[2] = 0.3; Scene->MonoLights[i].Specular[3] = 1.0;
	Scene->StateMonoLight[i] = 2;
	Scene->MonoLightForN3DS[i]=Number3DSObject;

	i=GLSCENE_MAXMONOLIGHTS; // Break the FOR
      }
}

void glSceneDeleteMonoLight(glSceneStruct *Scene, int NML)
{
  Scene->StateMonoLight[NML] = 0;
}

void glSceneEnableMonoLight(glSceneStruct *Scene, int NML)                     // Set StateMonoLight[]=2 
{
  if (Scene->StateMonoLight[NML] != 0)
    Scene->StateMonoLight[NML] = 2;
}

void glSceneDisableMonoLight(glSceneStruct *Scene, int NML)                    // Set StateMonoLight[]=1
{
  if (Scene->StateMonoLight[NML] != 0)
    Scene->StateMonoLight[NML] = 1;
}

void glSceneMonoLightSetPosition(glSceneStruct *Scene, int NML, float x, float y, float z)
{
  Scene->MonoLights[NML].Position[0] = x; Scene->MonoLights[NML].Position[1] = y; Scene->MonoLights[NML].Position[2] = z; 
}

void glSceneMonoLightSetAmbient( glSceneStruct *Scene, int NML, float a, float b, float c, float d)
{
  Scene->MonoLights[NML].Ambient[0] = a; Scene->MonoLights[NML].Ambient[1] = b; Scene->MonoLights[NML].Ambient[2] = c;  Scene->MonoLights[NML].Ambient[3] = d;
}

void glSceneMonoLightSetDiffuse(glSceneStruct *Scene, int NML, float a, float b, float c, float d)
{
  Scene->MonoLights[NML].Diffuse[0] = a; Scene->MonoLights[NML].Diffuse[1] = b; Scene->MonoLights[NML].Diffuse[2] = c;  Scene->MonoLights[NML].Diffuse[3] = d;
}

void glSceneMonoLightSetSpecular(glSceneStruct *Scene, int NML, float a, float b, float c, float d)
{
  Scene->MonoLights[NML].Specular[0] = a; Scene->MonoLights[NML].Specular[1] = b; Scene->MonoLights[NML].Specular[2] = c;  Scene->MonoLights[NML].Specular[3] = d;
}


void glSceneAddText(glSceneStruct *Scene, int x/*[0;100]*/, int y/*[0;100]*/,char* str, int R, int G, int B, int *NT)
{
  int i;
  *NT=0;
  for(i=0;i<GLSCENE_MAXTEXT;i++)
    if(Scene->StateText[i]==0)
      {
	Scene->StateText[i]=2;
	strcpy(Scene->Text[i],str);
	Scene->TextColor[i][0]=R;
	Scene->TextColor[i][1]=G;
	Scene->TextColor[i][2]=B;
	Scene->TextPos[i][0]=x;
	Scene->TextPos[i][1]=y;
	*NT = i;
	i=GLSCENE_MAXTEXT;
	break;
      }
}

void glSceneDeleteText(glSceneStruct *Scene, int NT)
{
      Scene->StateText[NT]=0;
      strcpy(Scene->Text[NT],"");
      Scene->TextColor[NT][0]=255;
      Scene->TextColor[NT][1]=0;
      Scene->TextColor[NT][2]=0;
      Scene->TextPos[NT][0]=10;
      Scene->TextPos[NT][1]=10;
}

void glSceneEnableText(glSceneStruct *Scene, int NT)
{
  if (Scene->StateText[NT]!=0)
    Scene->StateText[NT]=2;
}

void glSceneDisableText(glSceneStruct *Scene, int NT)
{
  if (Scene->StateText[NT]!=0)
    Scene->StateText[NT]=1;
}

void glSceneSetText(glSceneStruct *Scene, int NT, char* str)
{
  strcpy(Scene->Text[NT],str);
}

void glSceneSetTextPosition(glSceneStruct *Scene, int NT, int x/*[0;100]*/, int y/*[0;100]*/, char* str)
{
  strcpy(Scene->Text[NT],str);
  Scene->TextPos[NT][0]=x;
  Scene->TextPos[NT][1]=y;

}




void glSceneEnableLight(int light,float *Pos,float *Amb,float *Dif,float *Spe)
{
  float Pos2[4];
  Pos2[0] = -Pos[0];
  Pos2[1] = Pos[1];
  Pos2[2] = Pos[2];
  Pos2[3] = 0.0;

  switch(light)
    {
    case 0:
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT0, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT0, GL_SPECULAR, Spe);
      break;
	
    case 1:
      glEnable(GL_LIGHT1);
      glLightfv(GL_LIGHT1, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT1, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT1, GL_SPECULAR, Spe);
      break;
      
    case 2:
      glEnable(GL_LIGHT2);
      glLightfv(GL_LIGHT2, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT2, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT2, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT2, GL_SPECULAR, Spe);
      break;
      
    case 3:
      glEnable(GL_LIGHT3);
      glLightfv(GL_LIGHT3, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT3, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT3, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT3, GL_SPECULAR, Spe);
      break;
      
    case 4:
      glEnable(GL_LIGHT4);
      glLightfv(GL_LIGHT4, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT4, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT4, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT4, GL_SPECULAR, Spe);
      break;
      
    case 5:
      glEnable(GL_LIGHT5);
      glLightfv(GL_LIGHT5, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT5, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT5, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT5, GL_SPECULAR, Spe);
      break;
      
    case 6:
      glEnable(GL_LIGHT6);
      glLightfv(GL_LIGHT6, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT6, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT6, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT6, GL_SPECULAR, Spe);
      break;
      
    case 7:
      glEnable(GL_LIGHT7);
      glLightfv(GL_LIGHT7, GL_AMBIENT, Amb);
      glLightfv(GL_LIGHT7, GL_DIFFUSE, Dif);
      glLightfv(GL_LIGHT7, GL_POSITION, Pos2);
      glLightfv(GL_LIGHT7, GL_SPECULAR, Spe);
      break;
      
    default:
      break;
    }
}

void glSceneDraw(glSceneStruct *Scene)
{
  int i,j,current_light=0;
  
  glClearColor( .1f, .1f, .1f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_COLOR_MATERIAL);

  glEnable(GL_DEPTH_TEST); //depth test
  glEnable(GL_TEXTURE_2D); //texturing

  //set Camera
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  //glFrustum(-2.0,2.0,-1.5,1.5,1.0,20000.0);
  gluPerspective(60.0, 4.0/3.3, 1.0, 20000.0);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glRotatef(180.0,0,1,0);//cause opengl look at -z
  glMultMatrixf(Scene->CameraRotation);
  glTranslatef(Scene->CameraPosition[0],-Scene->CameraPosition[1],-Scene->CameraPosition[2]);

  //Draw Object
  glMatrixMode( GL_MODELVIEW );

  for(i=0;i<GLSCENE_MAX3DS;i++)
    if (Scene->State3DS[i]==2)
      {
		glLoadIdentity();

		//Position
		glTranslatef(-Scene->Obj3DSPosition[i][0],Scene->Obj3DSPosition[i][1],Scene->Obj3DSPosition[i][2]);

		//Rotation
		glMultMatrixf(Scene->Obj3DSRotation[i]);

		//Enable lights
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		current_light=0;
		if (Scene->Obj3DSUseGlobalLights[i])
		  {
			for(j=0;j<GLSCENE_MAXGLOBALLIGHTS;j++)
			  if (current_light<GLSCENE_MAXTOTALLIGHTS && Scene->StateGlobalLight[j]==2)
			{
			  glSceneEnableLight(current_light,Scene->GlobalLights[j].Position,Scene->GlobalLights[j].Ambient,Scene->GlobalLights[j].Diffuse,Scene->GlobalLights[j].Specular);
			  current_light++;
			}
		  }
		for(j=0;j<GLSCENE_MAXMONOLIGHTS;j++)
		  if (current_light<GLSCENE_MAXTOTALLIGHTS && Scene->MonoLightForN3DS[j]==i && Scene->StateMonoLight[j]==2)
			{
			  glSceneEnableLight(current_light,Scene->MonoLights[j].Position,Scene->MonoLights[j].Ambient,Scene->MonoLights[j].Diffuse,Scene->MonoLights[j].Specular);
			  current_light++;
			}

		//Draw 3ds
		glPushMatrix();
		glScalef(Scene->Obj3DSScale[i][0],Scene->Obj3DSScale[i][1],Scene->Obj3DSScale[i][2]);
		//glSceneDraw3DSCallList(&(Scene->Obj3DS[i]), Scene->gTexture[i], (GLuint)i+11, Scene->firsttime3DS[i]);//don t work => to fix
		glSceneDraw3DS(&(Scene->Obj3DS[i]), Scene->gTexture[i]);
		Scene->firsttime3DS[i]=0;
		glPopMatrix();
		//Disable lights
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
		glDisable(GL_LIGHT4);
		glDisable(GL_LIGHT5);
		glDisable(GL_LIGHT6);
		glDisable(GL_LIGHT7);

		// Draw Axis
		if (Scene->DrawAxis) 
				glSceneDrawAxis(Scene->Obj3DS[i].max*1.25);
      }

  glLoadIdentity();
//  glTranslatef(Scene->CameraPosition[0],-Scene->CameraPosition[1],-Scene->CameraPosition[2]);
 if (Scene->DrawAxis)
    glSceneDrawBase();

  for(i=0;i<GLSCENE_MAXTEXT;i++)
    if(Scene->StateText[i]==2)
      glSceneDrawText(Scene->TextPos[i][0],Scene->TextPos[i][1],Scene->Text[i],Scene->TextColor[i][0],Scene->TextColor[i][1],Scene->TextColor[i][2]);//dont work now// TODO
	
      
       /////////////////////////////////////////////////////////////////////////////////////

                                ///
                              //   //
                             //     //
                            //       //
                           //         //
                          //    !!!    //
                         //     !!!     //
                        //      !!!      //
                       //       !!!       //
                      //        !!!        //
                     //         !!!         //
                    //                       //
                   //           !!!           //
                  //                           //
                 /////////////////////////////////

  //AFTER CALLING THIS FUNCTION YOU MUST SWAP THE BUFFER OF THE WINDOW TO DRAW THE DATAS
  // Windows: SwapBuffers(HDC);
  // glut:    glutSwapBuffers();
  // linux:   depends of your graphical language

  ////////////////////////////////////////////////////////////////////////////////////////
 
}

void glSceneDraw3DSCallList(Loader3ds *My3DS, unsigned int *gTexture, GLuint VALUE_CALLLIST, int firsttime)
{
  int i,j,whichVertex;

  if (!glIsList(VALUE_CALLLIST))
   {
      glNewList(VALUE_CALLLIST,GL_COMPILE_AND_EXECUTE);

	if (My3DS->NBobjects>0) 
	{ 
		int MatIdCurrent=-1; 
		Objects *curobj; 
		curobj=My3DS->objects; 
 
		for(i = 0; i < My3DS->NBobjects; i++) 
		{ 
			glPushMatrix(); 
			// Check to see if this object has a texture map, if so bind the texture to it. 
			if(curobj->ID!=-1) { 
				// Turn on texture mapping and turn off color 
				glEnable(GL_TEXTURE_2D); 
				// Reset the color to normal again 
				glColor3ub(255, 255, 255); 
			} else { 
				// Turn off texture mapping and turn on color 
				glDisable(GL_TEXTURE_2D); 
				// Reset the color to normal again 
				glColor3ub(255, 255, 255); 
			} 
				// Go through all of the faces (polygons) of the object and draw them 
				for(j = 0; j < curobj->NBFaces; j++) 
				{ 
					//glEnable(GL_TEXTURE_2D); 
					if (curobj->MatID[j]!=MatIdCurrent)  
					{ 
						if (gTexture[curobj->MatID[j]]!=0) 
						{ 
							glBindTexture(GL_TEXTURE_2D, gTexture[curobj->MatID[j]]); 
							MatIdCurrent=curobj->MatID[j];  
						} 
					} 
 
				glBegin(GL_TRIANGLES);					// Begin drawing with our selected mode (triangles or lines) 
					// Go through each corner of the triangle and draw it. 
					for(whichVertex = 0; whichVertex < 3; whichVertex++) 
					{ 
						int index = curobj->Faces[j*3+whichVertex]; 
						glNormal3f(curobj->Normal[index*3], curobj->Normal[index*3+1],curobj->Normal[index*3+2]); 
						if(curobj->hastext)  
							glTexCoord2f(curobj->CoordTex[index*2], curobj->CoordTex[index*2+1]); 
						else 
							glColor3ub(curobj->color[0],curobj->color[1] ,curobj->color[2] );						 
						glVertex3f(curobj->vert[index*3], curobj->vert[index*3+1], curobj->vert[index*3+2]); 
					} 
				glEnd();	// End the drawing 
				} 
			glPopMatrix(); 
			curobj=curobj->next; 
		}	 
	}
       glEndList();
    }
  else
    {
		glCallList(VALUE_CALLLIST);
    }
}

void glSceneDraw3DS(Loader3ds *My3DS, unsigned int *gTexture)
{
  int i,j,whichVertex;

	if (My3DS->NBobjects>0) 
	{ 
		int MatIdCurrent=-1; 
		Objects *curobj; 
		curobj=My3DS->objects; 
 
		for(i = 0; i < My3DS->NBobjects; i++) 
		{ 
			glPushMatrix(); 
			// Check to see if this object has a texture map, if so bind the texture to it. 
			if(curobj->ID!=-1) { 
				// Turn on texture mapping and turn off color 
				glEnable(GL_TEXTURE_2D); 
				// Reset the color to normal again 
				glColor3ub(255, 255, 255); 
			} else { 
				// Turn off texture mapping and turn on color 
				glDisable(GL_TEXTURE_2D); 
				// Reset the color to normal again 
				glColor3ub(255, 255, 255); 
			} 
				// Go through all of the faces (polygons) of the object and draw them 
				for(j = 0; j < curobj->NBFaces; j++) 
				{ 
					//glEnable(GL_TEXTURE_2D); 
					if (curobj->MatID[j]!=MatIdCurrent)  
					{ 
						if (gTexture[curobj->MatID[j]]!=0) 
						{ 
							glBindTexture(GL_TEXTURE_2D, gTexture[curobj->MatID[j]]); 
							MatIdCurrent=curobj->MatID[j];  
						} 
					} 
 
				glBegin(GL_TRIANGLES);					// Begin drawing with our selected mode (triangles or lines) 
					// Go through each corner of the triangle and draw it. 
					for(whichVertex = 0; whichVertex < 3; whichVertex++) 
					{ 
						int index = curobj->Faces[j*3+whichVertex]; 
						glNormal3f(curobj->Normal[index*3], curobj->Normal[index*3+1],curobj->Normal[index*3+2]); 
						if(curobj->hastext)  
							glTexCoord2f(curobj->CoordTex[index*2], curobj->CoordTex[index*2+1]); 
						else 
							glColor3ub(curobj->color[0],curobj->color[1] ,curobj->color[2] );						 
						glVertex3f(curobj->vert[index*3], curobj->vert[index*3+1], curobj->vert[index*3+2]); 
					} 
				glEnd();	// End the drawing 
				} 
			glPopMatrix(); 
			curobj=curobj->next; 
		}	 
	}

}

/****************************************************************************************************************************/
/*********** Loading Textures   *************************/

void glSceneLoadTextures(Loader3ds *Obj3DS, char* DirectoryTexture, unsigned int *gTexture)
{
	char tempstr[200]; 
	int i,i2,ireceive=0; 
	Materials *it_mat,*it_mat2; 
 
	it_mat=Obj3DS->materials; 
	for(i = 0; i < Obj3DS->NBmaterials; i++) 
	{ 
		// Check to see if there is a file name to load in this material 
		if(strlen(it_mat->strFile) > 0) 
		{ 
			//if it s already load, don t load it again
			it_mat2=Obj3DS->materials; 
			ireceive=-1; 
			for(i2=0;i2<i;i2++) 
			{ 
				if(!strcmp(it_mat->strFile,it_mat2->strFile)) 
					{ 
						ireceive = i2; 
						gTexture[i]=gTexture[i2]; 
						i2=i; 
				} 
				it_mat2=it_mat2->next; 
			} 
			 
			if (ireceive<0) 
			{ 
			strcpy(tempstr,DirectoryTexture);
			strcat(tempstr,it_mat->strFile); 
			if (  ((tempstr[strlen(tempstr)-3]=='T') || (tempstr[strlen(tempstr)-3]=='t'))
			    &&((tempstr[strlen(tempstr)-2]=='G') || (tempstr[strlen(tempstr)-2]=='g'))
			    &&((tempstr[strlen(tempstr)-1]=='A') || (tempstr[strlen(tempstr)-1]=='a')))
			  gTexture[i] = glSceneCreateTextureTGA(tempstr);
			else if (  ((tempstr[strlen(tempstr)-3]=='B') || (tempstr[strlen(tempstr)-3]=='b'))
				 &&((tempstr[strlen(tempstr)-2]=='M') || (tempstr[strlen(tempstr)-2]=='m'))
				 &&((tempstr[strlen(tempstr)-1]=='P') || (tempstr[strlen(tempstr)-1]=='p')))
			gTexture[i] = glSceneCreateTextureBMP(tempstr);
			  else printf("unknow format image:%s\n",tempstr);
			ireceive = i; 
			} 
		} 
 
		// Set the texture ID for this material 
		it_mat->ID = ireceive; 
		it_mat=it_mat->next; 
	}
}

unsigned int glScenegetshort(FILE *fp) 
{ 
  int c, c1; 
   
  /* get 2 bytes*/ 
  c = getc(fp);   
  c1 = getc(fp); 
 
  return ((unsigned int) c) + (((unsigned int) c1) << 8); 
} 

unsigned int glScenegetint(FILE *fp) 
{ 
  int c, c1, c2, c3; 
 
  /*  get 4 bytes */  
  c = getc(fp);   
  c1 = getc(fp);   
  c2 = getc(fp);   
  c3 = getc(fp); 
 
  return ((unsigned int) c) + 
    (((unsigned int) c1) << 8) +  
    (((unsigned int) c2) << 16) + 
    (((unsigned int) c3) << 24); 
} 

unsigned int glSceneCreateTextureBMP(char *fileBMP)
{
  GLuint texture[10];
	 
    GLubyte     *image=NULL;   
    GLuint      imageSize; 
    GLsizei     Width,Height; 
    int         i; 
    unsigned short int planes;          /*  number of planes in image (must be 1)  */ 
    unsigned short int bpp;             /*  number of bits per pixel (must be 24) */ 
 
	texture[0]=0; 
    // Lit le fichier et son header 
    FILE * fichier = fopen(fileBMP,"rb"); 
    if (!fichier) {printf("%s introuvable\n\n",fileBMP); exit(0);}	 
    
       // Récupère les infos de l'image 
	fseek(fichier, 18, SEEK_CUR); 
	Width = glScenegetint(fichier); 
	Height = glScenegetint(fichier); 
    imageSize=Width*Height*3;   
	/*  read the planes */     
    planes = glScenegetshort(fichier); 
    if (planes != 1) { 
	printf("Planes from %s is not 1: %u\n", fileBMP, planes); 
	return 0; 
    } 
 
    /*  read the bpp */     
        bpp = glScenegetshort(fichier); 
    if (bpp != 24) { 
      printf("Bpp from %s is not 24: %u\n", fileBMP, bpp); 
      return 0; 
    } 
	fseek(fichier, 24, SEEK_CUR); 
 
 
    // Charge l'image 
    image = (GLubyte *) malloc ( imageSize ); 
    if (fread(image,1,imageSize,fichier)!=imageSize)  
     { 
        free (image); 
        {printf("%s erreur\n\n",fileBMP); exit(0);} 
     } 

    // Inverse R et B 
    for ( i = 0; i < (int)imageSize; i += 3 ) 
     { 
        int t = image[i]; 
        image[i] = image[i+2]; 
        image[i+2] = t; 	 
    } 

	glEnable(GL_TEXTURE_2D); 
	// Generate a texture with the associative texture ID stored in the array 
	glGenTextures(1, &texture[0]); 
	// This sets the alignment requirements for the start of each pixel row in memory. 
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1); 
	// Bind the texture to the texture arrays index and init the texture 
	glBindTexture(GL_TEXTURE_2D, texture[0]); 
	// Build Mipmaps (builds different versions of the picture for distances - looks better) 
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3/*GL_RGB*/, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, image); 
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); 

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//bloc dégradé flou 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//bloc ok 
	
	if (image)// If we loaded the bitmap 
		free(image);									// Free the bitmap structure 
	
	return texture[0];
}

unsigned int glSceneCreateTextureTGA(char *fileTGA)
{  
  unsigned int value=0;
  return (value);
}




void glSceneDrawAxis( float l )
{
  glDisable( GL_LIGHTING );

  glPushMatrix();

  glBegin( GL_LINES );
 
  glColor3f( 1.0, 0.0, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( -l, 0.0, 0.0 ); /* X axis      */

  glColor3f( 0.0, 1.0, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, l, 0.0 ); /* Y axis      */

  glColor3f( 0.0, 0.3, 1.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, l ); /* Z axis    */
  glEnd();

  glPopMatrix();

  glEnable( GL_LIGHTING );
}

void glSceneDrawBase()
{glTranslatef(20.0,0.0,0.0);
  //TODO: Draw a base XYZ front of the user (in the corner)
/*  glDisable( GL_LIGHTING );
  glBegin(GL_TRIANGLES);			
  glColor3f(1.0,0.0,0.0);
  glVertex3f(  0.0,  0.0,  0.0); 
  glVertex3f(  0.0,  2.0,  0.0); 
  glVertex3f( -10.0,  0.0,  0.0); 
  glVertex3f(  0.0,  2.0,  0.0); 
  glVertex3f( -10.0,  2.0,  0.0); 
  glVertex3f( -10.0,  0.0,  0.0); 
  glEnd();
  glEnable( GL_LIGHTING );*/
			
}

void glSceneBitmapCharacter(glSceneBitmapFontPtr font, int c)
{
  const glSceneBitmapCharRec *ch;
  glSceneBitmapFontPtr fontinfo;
  GLint swapbytes, lsbfirst, rowlength;
  GLint skiprows, skippixels, alignment;


  fontinfo = (glSceneBitmapFontPtr) font;


  if (c < fontinfo->first ||
    c >= fontinfo->first + fontinfo->num_chars)
    return;
  ch = fontinfo->ch[c - fontinfo->first];
  if (ch) {
    /* Save current modes. */
    glGetIntegerv(GL_UNPACK_SWAP_BYTES, &swapbytes);
    glGetIntegerv(GL_UNPACK_LSB_FIRST, &lsbfirst);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowlength);
    glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skiprows);
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skippixels);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
    /* Little endian machines (DEC Alpha for example) could
       benefit from setting GL_UNPACK_LSB_FIRST to GL_TRUE
       instead of GL_FALSE, but this would require changing the
       generated bitmaps too. */
    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBitmap(ch->width, ch->height, ch->xorig, ch->yorig,
      ch->advance, 0, ch->bitmap);
    /* Restore saved modes. */
    glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
    glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
  }
}

void glSceneDrawText(int x/*[0;100]*/, int y/*[0;100]*/,char* str, int R, int G, int B)
{
/*TODO*/
 /* static const glSceneBitmapCharRec * const chars[] = {};

  const glSceneBitmapFontPtr glSceneBitmapHelvetica18 = {
    "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1",
    224,
    32,
    chars
  };

  glDisable( GL_LIGHTING ); 
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0.0, 100.0, 0.0, 100.0  );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glColor3ub( R, G, B );
  glRasterPos2i( x, y );



  int i;
  for( i=0; i<(int)strlen( str); i++ )
    glSceneBitmapCharacter( glSceneBitmapHelvetica18 , str[i] );

  glEnable( GL_LIGHTING );*/
}







