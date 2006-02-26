/*

$Id: Loader3ds.h,v 1.2 2006-02-26 13:36:05 erk Exp $

-----------------------------------------------------------------------

Visu3D Library - core components for a 3D Software Visualisation.

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
Maintainer : olivier monaret
Component : Visu3D :: Loader 3DS

-----------------------------------------------------------------------

Purpose   : Loader 3DS

-----------------------------------------------------------------------
 */

//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#define PI 3.14159 

//>------ Primary Chunk, at the beginning of each file
#define PRIMARY       0x4D4D

//>------ Main Chunks
#define OBJECTINFO    0x3D3D				// This gives the version of the mesh
#define VERSION       0x0002				// This gives the version of the .3ds file 
#define EDITKEYFRAME  0xB000				// This is the header for all of the key frame info 
 
//>------ sub defines of OBJECTINFO 
#define MATERIAL	  0xAFFF				// This stored the texture info 
#define OBJECT		  0x4000				// This stores the faces, vertices, etc... 
 
//>------ sub defines of MATERIAL 
#define MATNAME       0xA000				// This holds the material name 
#define MATDIFFUSE    0xA020				// This holds the color of the object/material 
#define MATTRANSP     0xA050				// Transparence 
#define MATMAP        0xA200				// This is a header for a new material 
#define MATMAPFILE    0xA300				// This holds the file name of the texture 
#define MATMAPVSCALE  0xA354				//  
#define MATMAPUSCALE  0xA356				//  
#define MATMAPUOFFSET 0xA358				//  
#define MATMAPVOFFSET 0xA35A				//  
 
#define OBJECT_MESH   0x4100				// This lets us know that we are reading a new object 
 
//>------ sub defines of OBJECT_MESH 
#define OBJECT_VERTICES     0x4110			// The objects vertices 
#define OBJECT_FACES		0x4120			// The objects faces 
#define OBJECT_MATERIAL		0x4130			// This is found if the object has a material, either texture map or color 
#define OBJECT_UV			0x4140			// The UV texture coordinates 
#define OBJECT_TRANSF		0x4160			// The transformation 
 
 
 
 
 
// Here is our structure for our 3DS indicies (since .3DS stores 4 unsigned shorts) 
typedef struct tIndices {							 
 
	unsigned short a, b, c, bVisible;		// This will hold point1, 2, and 3 index's into the vertex array plus a visible flag 
}tIndices; 
 
// This holds the chunk info 
typedef struct tChunk 
{ 
	unsigned short int ID;					// The chunk's ID		 
	unsigned int length;					// The length of the chunk 
	unsigned int bytesRead;					// The amount of bytes read within that chunk 
}tChunk; 
 
typedef struct _Materials 
{ 
	char strFile[255]; 
	char Name[255]; 
	unsigned char color[3]; 
	int ID; 
	float uTile; 
	float vTile; 
	float uOffset; 
	float vOffset; 
	int transparence; 
	struct _Materials *next; 
}Materials; 
 
typedef struct _Objects 
{ 
	char Name[255]; 
	int NBvert; 
	int NBFaces; 
	int NBCoordTex; 
	float *vert; 
	float *transf; 
	int *Faces; 
	float *CoordTex; 
	float *Normal; 
	int ID; 
	int *MatID; 
	int hastext; 
	unsigned char color[3]; 
	float scale[3]; 
	float rotation[3]; 
 
	struct _Objects *next; 
}Objects; 
 
 
typedef struct Loader3ds
{
	FILE *FilePointer; 
	int NBmaterials; 
	int NBobjects; 
 
	Materials *materials; 
	Objects *objects; 
	Materials *curmat; 
	Objects *curobj; 
	unsigned char gBuffer[50000]; 
	float max; 
}Loader3ds;


	void CreateLoader3ds(Loader3ds *pt3ds);  
	// This is the function that you call to load the 3DS 
	int Import3DS(Loader3ds *pt3ds, char *strFileName); 
	void ReBoot(Loader3ds *pt3ds);  
	void DestroyLoader3ds(Loader3ds *pt3ds); 
	int PrepareAddObject(Loader3ds *pt3ds); 
	int PrepareAddMaterial(Loader3ds *pt3ds); 
 
	// This reads in a string and saves it in the char array passed in 
	int GetString(Loader3ds *pt3ds,char *); 
 
	// This reads the next chunk 
	void ReadChunk(Loader3ds *pt3ds,tChunk *); 
 
	// This reads the next large chunk 
	void ProcessNextChunk(Loader3ds *pt3ds, tChunk *); 
 
	// This reads the object chunks 
	void ProcessNextObjectChunk(Loader3ds *pt3ds,tChunk *); 
 
	// This reads the material chunks 
	void ProcessNextMaterialChunk(Loader3ds *pt3ds,tChunk *); 
 
	// This reads the RGB value for the object's color 
	void ReadColorChunk(Loader3ds *pt3ds,tChunk *pChunk); 
 
	// This reads the objects vertices 
	void ReadVertices(Loader3ds *pt3ds,tChunk *); 
 
	// This reads the objects face information 
	void ReadVertexIndices(Loader3ds *pt3ds,tChunk *); 
 
	// This reads the texture coodinates of the object 
	void ReadUVCoordinates(Loader3ds *pt3ds,tChunk *); 
 
	// This reads the texture coodinates of the object 
	void ReadObjectTransf(Loader3ds *pt3ds,tChunk *); 
 
	// This reads in the material name assigned to the object and sets the materialID 
	void ReadObjectMaterial(Loader3ds *pt3ds,tChunk *pPreviousChunk); 
	 
	// This computes the vertex normals for the object (used for lighting) 
	void ComputeNormals(Loader3ds *pt3ds); 
 
 
///////////////////////////////////////////////////////////////////////////// 

