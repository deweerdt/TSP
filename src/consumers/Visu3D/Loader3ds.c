/*!  \file 

$Id: Loader3ds.c,v 1.1 2005-03-29 22:42:20 erk Exp $

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
 
#include "Loader3ds.h" 
 
///////////////////////////////////////////////////////////////////////////// 
// Loader3ds 
///////////////////////////////// CLoader3ds \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
///// 
/////	This constructor initializes the tChunk data 
///// 
///////////////////////////////// CLoader3ds \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void CreateLoader3ds(Loader3ds *pt3ds) 
{ 
	pt3ds->FilePointer = NULL; 
	pt3ds->materials = NULL; 
	pt3ds->objects = NULL; 
	pt3ds->curmat=NULL; 
	pt3ds->curobj=NULL; 
	pt3ds->NBmaterials=0; 
	pt3ds->NBobjects=0; 
	pt3ds->max=0.0; 
 
	 
} 
void DestroyLoader3ds(Loader3ds *pt3ds) 
{ 
 	Materials *maux,*maux2; 
	Objects *oaux,*oaux2; 

 
	if (pt3ds->FilePointer != NULL) fclose(pt3ds->FilePointer); 
 
	maux=pt3ds->materials; 
	while(maux!=NULL) 
	{ 
		maux2=maux->next; 
		if (maux) free(maux); 
		maux=maux2; 
	} 
 
	oaux=pt3ds->objects; 
	while(oaux!=NULL) 
	{ 
		oaux2=oaux->next; 
		if (oaux->vert) free(oaux->vert); 
		if (oaux->transf) free(oaux->transf); 
		if (oaux->Faces) free(oaux->Faces); 
		if (oaux->MatID) free(oaux->MatID); 
		if (oaux->Normal) free(oaux->Normal); 
		if (oaux->CoordTex) free(oaux->CoordTex); 
		if (oaux) free(oaux); 
		oaux=oaux2; 
	} 
	 
	 
} 
 
void ReBoot(Loader3ds *pt3ds) 
{ 
	Materials *maux,*maux2; 
	Objects *oaux,*oaux2; 

	if (pt3ds->FilePointer != NULL) fclose(pt3ds->FilePointer); 
 
	maux=pt3ds->materials; 
	while(maux!=NULL) 
	{ 
		maux2=maux->next; 
		if (maux) free(maux); 
		maux=maux2; 
	} 
 
	oaux=pt3ds->objects; 
	while(oaux!=NULL) 
	{ 
		oaux2=oaux->next; 
		if (oaux->vert) free(oaux->vert); 
		if (oaux->transf) free(oaux->transf); 
		if (oaux->Faces) free(oaux->Faces); 
		if (oaux->MatID) free(oaux->MatID); 
		if (oaux->Normal) free(oaux->Normal); 
		if (oaux->CoordTex) free(oaux->CoordTex); 
		if (oaux) free(oaux); 
		oaux=oaux2; 
	} 
	pt3ds->FilePointer = NULL; 
	pt3ds->materials = NULL; 
	pt3ds->objects = NULL; 
	pt3ds->curmat=NULL; 
	pt3ds->curobj=NULL; 
	pt3ds->NBmaterials=0; 
	pt3ds->NBobjects=0; 
	pt3ds->max=0.0; 
 
 
} 
 
int PrepareAddObject(Loader3ds *pt3ds) 
{ 
	if(pt3ds->objects == NULL) 
        { 
          pt3ds->objects = (Objects*)malloc(sizeof(Objects)); 
          pt3ds->curobj = pt3ds->objects; 
        } 
        else 
        { 
          pt3ds->curobj->next = (Objects*)malloc(sizeof(Objects)); 
          pt3ds->curobj = pt3ds->curobj->next; 
        } 
				 
	pt3ds->curobj->NBvert=0; 
	pt3ds->curobj->vert=NULL; 
	pt3ds->curobj->transf=NULL; 
	pt3ds->curobj->NBFaces=0; 
	pt3ds->curobj->Faces=NULL; 
	pt3ds->curobj->NBCoordTex=0; 
	pt3ds->curobj->Normal=NULL; 
	pt3ds->curobj->CoordTex=NULL; 
	pt3ds->curobj->ID=-1; 
	pt3ds->curobj->MatID=NULL; 
	pt3ds->curobj->next=NULL; 
	pt3ds->curobj->hastext=0; 
	pt3ds->curobj->scale[0]=1.0; 
	pt3ds->curobj->scale[1]=1.0; 
	pt3ds->curobj->scale[2]=1.0; 
	return (1); 
} 
 
int PrepareAddMaterial(Loader3ds *pt3ds) 
{ 
	if(pt3ds->materials == NULL) 
        { 
          pt3ds->materials = (Materials*)malloc(sizeof(Materials)); 
          pt3ds->curmat = pt3ds->materials; 
        } 
        else 
        { 
          pt3ds->curmat->next = (Materials*)malloc(sizeof(Materials)); 
          pt3ds->curmat = pt3ds->curmat->next; 
        } 
				 
	pt3ds->curmat->ID=0; 
	pt3ds->curmat->uTile=0.0; 
	pt3ds->curmat->vTile=0.0; 
	pt3ds->curmat->uOffset=0.0; 
	pt3ds->curmat->vOffset=0.0; 
	pt3ds->curmat->transparence=0; 
	pt3ds->curmat->next=NULL; 
	strcpy(pt3ds->curmat->strFile,""); 
	return (1); 
} 
 
///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
/////	This is called to open the .3ds file
///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
int Import3DS(Loader3ds *pt3ds,char *strFileName) 
{
 
	char strMessage[255] = {0}; 
	tChunk currentChunk = {0}; 
 
	// Open the 3DS file 
	pt3ds->FilePointer = fopen(strFileName, "rb"); 
 
	// Make sure we have a valid file pointer
	if(!pt3ds->FilePointer)  
	{ 
		sprintf(strMessage, "Unable to find the file: %s!", strFileName); 
		printf("%s\n",strMessage);
		return 0; 
	} 
 
	// Read the first chuck of the file to see if it's a 3DS file 
	ReadChunk(pt3ds,&currentChunk); 
 
	// Make sure this is a 3DS file 
	if (currentChunk.ID != PRIMARY) 
	{ 
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", strFileName); 
		printf("%s\n",strMessage); 
		return 0; 
	} 

	//ProcessNextChunk() is recursive 
	ProcessNextChunk(pt3ds,&currentChunk); 
 
	// calculate vertex normals. 
	ComputeNormals(pt3ds); 
 
	if (pt3ds->FilePointer != NULL) fclose(pt3ds->FilePointer); 
 
	return 1; 
} 
 
 
 
///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
/////	This function reads the main sections of the .3DS file
///////////////////////////////// PROCESS NEXT CHUNK\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ProcessNextChunk(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{ 
 
	tChunk currentChunk = {0};					// The current chunk to load 
	tChunk tempChunk = {0};						// A temp chunk for holding data		 
 
	while (pPreviousChunk->bytesRead < pPreviousChunk->length) 
	{ 
		// Read next Chunk 
		ReadChunk(pt3ds,&currentChunk); 
		// Check the chunk ID 
		switch (currentChunk.ID) 
		{ 
		case VERSION:			    
			// Read the file version 
			currentChunk.bytesRead += fread(pt3ds->gBuffer, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
 
			if ((currentChunk.length - currentChunk.bytesRead == 4) && (pt3ds->gBuffer[0] > 0x03)) { 
				printf("This 3DS file is over version 3 so it may load incorrectly\n");		 
			} 
			break; 
 
		case OBJECTINFO:				   
 
			{	 
			// This chunk holds the version of the mesh.  
 
			// Read the next chunk 
			ReadChunk(pt3ds,&tempChunk); 
 
			// Get the version of the mesh 
			tempChunk.bytesRead += fread(pt3ds->gBuffer, 1, tempChunk.length - tempChunk.bytesRead, pt3ds->FilePointer); 
 
			// Increase the bytesRead by the bytes read from the last chunk 
			currentChunk.bytesRead += tempChunk.bytesRead; 
 
			// Go to the next chunk, which is the object has a texture, it should be MATERIAL, then OBJECT. 
			ProcessNextChunk(pt3ds,&currentChunk); 
			break; 
		} 
		case MATERIAL:
 
			// This chunk is the header for the material info chunks 
 
			// Increase the number of materials 
			pt3ds->NBmaterials++; 
	 
			PrepareAddMaterial(pt3ds); 
 
			// Proceed to the material loading function 
			ProcessNextMaterialChunk(pt3ds,&currentChunk); 
			break; 
 
		case OBJECT:
			// This chunk is the header for the object info chunks.

			// Increase the object count 
			pt3ds->NBobjects++; 

			PrepareAddObject(pt3ds); 
			// Get the name of the object
			currentChunk.bytesRead += GetString(pt3ds,pt3ds->curobj->Name); 
			 
			// Now proceed to read in the rest of the object information 
			ProcessNextObjectChunk(pt3ds,&currentChunk); 
			break; 
 
/*		case EDITKEYFRAME: 
			pt3ds->curobj=objects; 
			ProcessNextChunk(pt3ds,&currentChunk); 
			break; 
 
		case 0xB002: 
			ProcessNextChunk(pt3ds,&currentChunk); 
			if (pt3ds->curobj!=NULL) 
				pt3ds->curobj = pt3ds->curobj->next; 
			break; 
		case 0xB030: 
			float tempf[9];int y; 
			short tempshort; 
			currentChunk.bytesRead += fread(&tempshort, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
		case 0xB010: 
			currentChunk.bytesRead += fread(gBuffer, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
		case 0xB013: 
			currentChunk.bytesRead += fread(tempf, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
		case 0xB020: 
			currentChunk.bytesRead += fread(tempf, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
		case 0xB021: 
			currentChunk.bytesRead += fread(tempf, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			pt3ds->curobj->rotation[0]=tempf[5]*(180.0/PI); 
			pt3ds->curobj->rotation[1]=tempf[6]*(180.0/PI); 
			pt3ds->curobj->rotation[2]=tempf[8]*(180.0/PI); 
			break; 
		case 0xB022: // INFO ROTATION SCALE ??? ->5,6 and 7 
			currentChunk.bytesRead += fread(tempf, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			if (tempf[5]==-1.0) pt3ds->curobj->scale[0]=-1.0; else pt3ds->curobj->scale[0]=1.0;  
			if (tempf[7]==-1.0) pt3ds->curobj->scale[1]=-1.0; else pt3ds->curobj->scale[1]=1.0;  
			if (tempf[6]==-1.0) pt3ds->curobj->scale[2]=-1.0; else pt3ds->curobj->scale[2]=1.0;  
			break;
*/
 
		default:  
			currentChunk.bytesRead += fread(pt3ds->gBuffer, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
		} 
 
		pPreviousChunk->bytesRead += currentChunk.bytesRead; 
	} 
} 
 
 
///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ProcessNextObjectChunk(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{
	tChunk currentChunk = {0}; 
 
	while (pPreviousChunk->bytesRead < pPreviousChunk->length) 
	{ 
		// Read the next chunk 
		ReadChunk(pt3ds,&currentChunk); 
		switch (currentChunk.ID) 
		{ 
		case OBJECT_MESH:
 
			// We found a new object so use recursion 
			ProcessNextObjectChunk(pt3ds,&currentChunk); 
			break; 
 
		case OBJECT_VERTICES:				// 4110This is the objects vertices 
 
			ReadVertices(pt3ds,&currentChunk); 
			break; 
 
		case OBJECT_FACES:					// 4120This is the objects face information 
 
			ReadVertexIndices(pt3ds,&currentChunk); 
			break; 
 
		case OBJECT_MATERIAL:		

			ReadObjectMaterial(pt3ds,&currentChunk);			 
			break; 
 
		case OBJECT_UV:						// 4140This holds the UV texture coordinates for the object 
 
			ReadUVCoordinates(pt3ds,&currentChunk); 
			break; 
 
		case OBJECT_TRANSF:					// 4160 
 
			ReadObjectTransf(pt3ds,&currentChunk); 
			break; 
 
		default:   
 
			currentChunk.bytesRead += fread(pt3ds->gBuffer, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
		} 
 
		pPreviousChunk->bytesRead += currentChunk.bytesRead; 
	} 
} 
 
 
///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 

void ProcessNextMaterialChunk(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{ 
	tChunk currentChunk = {0}; 
	char chartemp; 
 
	while (pPreviousChunk->bytesRead < pPreviousChunk->length) 
	{ 
		// Read the next chunk 
		ReadChunk(pt3ds,&currentChunk); 
		switch (currentChunk.ID) 
		{ 
		case MATNAME:							
		        // This chunk holds the name of the material 
			currentChunk.bytesRead += fread(pt3ds->curmat->Name, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			pt3ds->curmat->uOffset=0.0; 
			pt3ds->curmat->vOffset=0.0; 
			pt3ds->curmat->uTile=1.0; 
			pt3ds->curmat->vTile=1.0; 
 
			break; 
 
		case MATDIFFUSE:						// This holds the R G B color of our object 
			ReadColorChunk(pt3ds,&currentChunk); 
		break; 
		 
		case MATMAP:
			 
			// Proceed to read in the material information 
			ProcessNextMaterialChunk(pt3ds,&currentChunk); 
			 
			break; 
 
		case MATMAPFILE:
 
			// Here we read in the material's file name 
			currentChunk.bytesRead += fread(pt3ds->curmat->strFile, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer);
		  	break; 
 
		case MATMAPVSCALE:
 
			// Here we read in the material's v Scale 
			currentChunk.bytesRead += fread(&(pt3ds->curmat->vTile), 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
 
		case MATMAPUSCALE:
 
			// Here we read in the material's u Scale 
			currentChunk.bytesRead += fread(&(pt3ds->curmat->uTile), 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
 
		case MATMAPUOFFSET:
 
			// Here we read in the material's u Offset 
			currentChunk.bytesRead += fread(&(pt3ds->curmat->uOffset), 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
 
		case MATMAPVOFFSET:
 
			// Here we read in the material's v Offset 
			currentChunk.bytesRead += fread(&(pt3ds->curmat->vOffset), 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
 
		case MATTRANSP:						// Transparence 0= no transp; 100=full transp 
			currentChunk.bytesRead += fread(pt3ds->gBuffer, 1, 6, pt3ds->FilePointer); 
			currentChunk.bytesRead += fread(&chartemp, 1, 1, pt3ds->FilePointer); 
			currentChunk.bytesRead += fread(pt3ds->gBuffer, 1, 1, pt3ds->FilePointer); 
			pt3ds->curmat->transparence = 100-chartemp; 
			break; 
 
		default:   
 
			currentChunk.bytesRead += fread(pt3ds->gBuffer, 1, currentChunk.length - currentChunk.bytesRead, pt3ds->FilePointer); 
			break; 
		} 
 
		pPreviousChunk->bytesRead += currentChunk.bytesRead; 
	} 
} 
 
///////////////////////////////// READ CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ReadChunk(Loader3ds *pt3ds,tChunk *pChunk) 
{ 
	pChunk->bytesRead = fread(&pChunk->ID, 1, 2, pt3ds->FilePointer); 
 	pChunk->bytesRead += fread(&pChunk->length, 1, 4, pt3ds->FilePointer); 
} 
 
///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
int GetString(Loader3ds *pt3ds,char *pBuffer) 
{ 
	int index = 0; 
 
	fread(pBuffer, 1, 1, pt3ds->FilePointer); 
 
	// Loop until we get NULL 
	while (*(pBuffer + index++) != 0) { 
		fread(pBuffer + index, 1, 1, pt3ds->FilePointer); 
	} 
 
	// Return the string length
	return strlen(pBuffer) + 1; 
} 
 
 
///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ReadColorChunk(Loader3ds *pt3ds,tChunk *pChunk) 
{ 
	tChunk tempChunk = {0}; 
 
	// Read the color chunk info 
	ReadChunk(pt3ds,&tempChunk); 
 
	tempChunk.bytesRead += fread(pt3ds->curmat->color, 1, tempChunk.length - tempChunk.bytesRead, pt3ds->FilePointer); 
 
	pChunk->bytesRead += tempChunk.bytesRead; 
} 
 
 
///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ReadVertexIndices(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{ 
	unsigned short index = 0;
	int i,j;
 
	// Read in the number of faces 
	pPreviousChunk->bytesRead += fread(&pt3ds->curobj->NBFaces, 1, 2, pt3ds->FilePointer); 
	pt3ds->curobj->Faces = (int*)malloc(sizeof(int) * (pt3ds->curobj->NBFaces*3));	 
	memset(pt3ds->curobj->Faces, 0, sizeof(float) * 3 * pt3ds->curobj->NBFaces); 
	pt3ds->curobj->MatID = (int*)malloc (sizeof(int) *(pt3ds->curobj->NBFaces)); 
 
	for(i = 0; i < pt3ds->curobj->NBFaces; i++) 
	{ 
		pt3ds->curobj->MatID[i] = 0; 
		for(j = 0; j < 4; j++) 
		{ 
			pPreviousChunk->bytesRead += fread(&index, 1, sizeof(index), pt3ds->FilePointer); 
 
			if(j < 3) 
			{ 
				pt3ds->curobj->Faces[i*3+j] = index; 
			} 
		} 
	} 
} 
 
 
///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ReadUVCoordinates(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{ 
 
	// Read in the number of UV coordinates there are (int)	 
	pPreviousChunk->bytesRead += fread(&pt3ds->curobj->NBCoordTex, 1, 2, pt3ds->FilePointer); 
 
	pt3ds->curobj->hastext=1; 
 
	pt3ds->curobj->CoordTex = (float*)malloc(sizeof(float) *(2 * pt3ds->curobj->NBCoordTex)); 
 
	pPreviousChunk->bytesRead += fread(pt3ds->curobj->CoordTex, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, pt3ds->FilePointer); 
 
} 
 
///////////////////////////////// READ TRANSF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ReadObjectTransf(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{ 
	// Allocate memory  
	pt3ds->curobj->transf = (float*)malloc(sizeof(float) * 16); 
 
	// Read in  
	pPreviousChunk->bytesRead += fread(pt3ds->curobj->transf, 1, 48, pt3ds->FilePointer); 
	pt3ds->curobj->transf[12]=0.0; 
	pt3ds->curobj->transf[13]=0.0; 
	pt3ds->curobj->transf[14]=0.0; 
	pt3ds->curobj->transf[15]=1.0; 
} 
 
///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ReadVertices(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{ 
  int i;

	pPreviousChunk->bytesRead += fread(&(pt3ds->curobj->NBvert), 1, 2, pt3ds->FilePointer); 
 
	pt3ds->curobj->vert = (float*)malloc(sizeof(float) * (3*pt3ds->curobj->NBvert)); 
 
	memset(pt3ds->curobj->vert, 0, sizeof(float) * 3 * pt3ds->curobj->NBvert); 
 
	pPreviousChunk->bytesRead += fread(pt3ds->curobj->vert, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, pt3ds->FilePointer); 
 
	for(i = 0; i < pt3ds->curobj->NBvert; i++) 
	{ 
	  //convert 3dsmax studio format to opengl
		float fTempY = pt3ds->curobj->vert[i*3+1]; 
 
		pt3ds->curobj->vert[i*3+1] = pt3ds->curobj->vert[i*3+2]; 
 
		pt3ds->curobj->vert[i*3+2] =   - fTempY; 
 
		if (fabs(pt3ds->curobj->vert[i*3])>pt3ds->max) pt3ds->max=(float)fabs(pt3ds->curobj->vert[i*3]); 
		if (fabs(pt3ds->curobj->vert[i*3+1])>pt3ds->max) pt3ds->max=(float)fabs(pt3ds->curobj->vert[i*3+1]); 
		if (fabs(pt3ds->curobj->vert[i*3+2])>pt3ds->max) pt3ds->max=(float)fabs(pt3ds->curobj->vert[i*3+2]); 
	} 
} 
 
 
///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ReadObjectMaterial(Loader3ds *pt3ds,tChunk *pPreviousChunk) 
{
int i;
	char strMaterial[255] = {0};
 	Materials *iter_mat; 
	unsigned short nbface=0; 
	unsigned short face=0; 

	pPreviousChunk->bytesRead += GetString(pt3ds,strMaterial); 
	iter_mat=pt3ds->materials; 
	for(i = 0; i < pt3ds->NBmaterials; i++) 
	{ 
		if(strcmp(strMaterial, iter_mat->Name) == 0) 
		{ 
			pt3ds->curobj->ID = i; 
 
			if(strlen(iter_mat->strFile) > 0) { 
 
				// Set the object's flag to say it has a texture map to bind. 
				//pt3ds->curobj->hastext = true; 
			} 
			else 
			{ 
				pt3ds->curobj->color[0]=iter_mat->color[0]; 
				pt3ds->curobj->color[1]=iter_mat->color[1]; 
				pt3ds->curobj->color[2]=iter_mat->color[2]; 
			} 
			break; 
		} 
		else 
		{ 
			// Set the ID to -1 to show there is no material for this object 
			pt3ds->curobj->ID = -1; 
		} 
		iter_mat = iter_mat->next; 
	} 
 
	nbface=0; 
	face=0; 
	pPreviousChunk->bytesRead += fread(&nbface,1,sizeof(unsigned short),pt3ds->FilePointer); 
	for(i = 0; i < nbface; i++) 
	{	 
		pPreviousChunk->bytesRead += fread(&face,1,sizeof(unsigned short),pt3ds->FilePointer); 
		pt3ds->curobj->MatID[face]=pt3ds->curobj->ID; 
	} 
}		 
 
// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2) 
double Mag(float N1,float N2,float N3) {return (sqrt(N1*N1 + N2*N2 + N3*N3));} 
 
// This calculates a vector between 2 points and returns the result 
void Vector(   float v1x,float v1y, float v1z, 
			   float v2x,float v2y, float v2z, 
			   float *rx,float *ry, float *rz) 
{ 
	*rx = v1x - v2x;			// Subtract point1 and point2 x's 
	*ry = v1y - v2y;			// Subtract point1 and point2 y's 
	*rz = v1z - v2z;			// Subtract point1 and point2 z's 
} 
 
// This adds 2 vectors together and returns the result 
void AddVector(float v1x,float v1y, float v1z, 
			   float v2x,float v2y, float v2z, 
			   float *rx,float *ry, float *rz) 
{	 
	*rx = v2x + v1x;		// Add Vector1 and Vector2 x's 
	*ry = v2y + v1y;		// Add Vector1 and Vector2 y's 
	*rz = v2z + v1z;		// Add Vector1 and Vector2 z's						 
} 
 
// This divides a vector by a single number (scalar) and returns the result 
void DivideVectorByScaler(float v1x,float v1y, float v1z, 
						  float *rx,float *ry, float *rz, float Scaler) 
{	 
	if (Scaler==0) 
	{ 
	*rx = 1.0;			// Divide Vector1's x value by the scaler 
	*ry = 0.0;			// Divide Vector1's y value by the scaler 
	*rz = 0.0;			// Divide Vector1's z value by the scaler 
	} 
	else 
	{ 
	*rx = v1x / Scaler;			// Divide Vector1's x value by the scaler 
	*ry = v1y / Scaler;			// Divide Vector1's y value by the scaler 
	*rz = v1z / Scaler;			// Divide Vector1's z value by the scaler 
	} 
} 
 
// This returns the cross product between 2 vectors 
void Cross(	float v1x,float v1y, float v1z, 
			float v2x,float v2y, float v2z, 
			float *rx,float *ry, float *rz) 
{												// Get the X value 
	*rx = ((v1y * v2z) - (v1z * v2y)); 
												// Get the Y value 
	*ry = ((v1z * v2x) - (v1x * v2z)); 
												// Get the Z value 
	*rz = ((v1x * v2y) - (v1y * v2x)); 
 
} 
 
// This returns the normal of a vector 
void Normalize(float *x,float *y, float *z) 
{ 
	double Magnitude;							// This holds the magitude			 
 
	Magnitude = (double)Mag(*x,*y,*z);					// Get the magnitude 
 
	*x /= (float)Magnitude;				// Divide the vector's X by the magnitude 
	*y /= (float)Magnitude;				// Divide the vector's Y by the magnitude 
	*z /= (float)Magnitude;				// Divide the vector's Z by the magnitude 
} 
 
///////////////////////////////// COMPUTER NORMALS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\* 
 
void ComputeNormals(Loader3ds *pt3ds) 
{ 
 int i,j,index; 
 int shared=0; 
	float	v1x=0.0,v1y=0.0,v1z=0.0, 
			v2x=0.0,v2y=0.0,v2z=0.0, 
			vnx=0.0,vny=0.0,vnz=0.0, 
			vp0x=0.0,vp0y=0.0,vp0z=0.0, 
			vp1x=0.0,vp1y=0.0,vp1z=0.0, 
			vp2x=0.0,vp2y=0.0,vp2z=0.0, 
			*pNormals=NULL, 
			*pTempNormals=NULL, 
			vSumx = 0.0,vSumy = 0.0,vSumz = 0.0;	 
 	Objects *cur_obj; 

	// If there are no objects, we can skip this part 
	if(pt3ds->NBobjects <= 0) 
		return; 
 
	// Go through each of the objects to calculate their normals 
	cur_obj = pt3ds->objects; 
	for(index = 0; index < pt3ds->NBobjects; index++) 
	{ 
		pNormals		= (float*)malloc(sizeof(float) *(3*cur_obj->NBFaces)); 
		pTempNormals	= (float*)malloc(sizeof(float) * (3*cur_obj->NBFaces)); 
		cur_obj->Normal = (float*)malloc(sizeof(float) * (3*cur_obj->NBvert)); 
 
 
		// Go though all of the faces of this object 
		for(i=0; i < cur_obj->NBFaces; i++) 
		  {
			vp0x=cur_obj->vert[0+3*cur_obj->Faces[i*3]]; 
			vp0y=cur_obj->vert[1+3*cur_obj->Faces[i*3]]; 
			vp0z=cur_obj->vert[2+3*cur_obj->Faces[i*3]]; 
 
			vp1x=cur_obj->vert[0+3*cur_obj->Faces[i*3+1]]; 
			vp1y=cur_obj->vert[1+3*cur_obj->Faces[i*3+1]]; 
			vp1z=cur_obj->vert[2+3*cur_obj->Faces[i*3+1]]; 
 
			vp2x=cur_obj->vert[0+3*cur_obj->Faces[i*3+2]]; 
			vp2y=cur_obj->vert[1+3*cur_obj->Faces[i*3+2]]; 
			vp2z=cur_obj->vert[2+3*cur_obj->Faces[i*3+2]]; 
  
			Vector(	vp0x,vp0y,vp0z, 
					vp2x,vp2y,vp2z, 
					&v1x,&v1y,&v1z);		// Get the vector of the polygon (we just need 2 sides for the normal) 
 
			Vector(	vp2x,vp2y,vp2z, 
					vp1x,vp1y,vp1z, 
					&v2x,&v2y,&v2z);		// Get a second vector of the polygon 
			 
			Cross(	v1x,v1y,v1z, 
					v2x,v2y,v2z, 
					&vnx,&vny,&vnz);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector) 
 
			pTempNormals[3*i] = vnx;					// Save the un-normalized normal for the vertex normals 
			pTempNormals[3*i+1] = vny;					// Save the un-normalized normal for the vertex normals 
			pTempNormals[3*i+2] = vnz;					// Save the un-normalized normal for the vertex normals 
 
			Normalize(&vnx,&vny,&vnz);				// Normalize the cross product to give us the polygons normal 
 
			pNormals[3*i] = vnx;						// Assign the normal to the list of normals 
			pNormals[3*i+1] = vny;						// Assign the normal to the list of normals 
			pNormals[3*i+2] = vnz;						// Assign the normal to the list of normals 
		} 
 
		//////////////// Now Get The Vertex Normals ///////////////// 
 
		vSumx = 0.0;vSumy = 0.0;vSumz = 0.0;		 
		shared=0; 
 
		for (i = 0; i < cur_obj->NBvert; i++)			// Go through all of the vertices 
		{ 
			for (j = 0; j < cur_obj->NBFaces; j++)	// Go through all of the triangles 
			{												// Check if the vertex is shared by another face 
				if (cur_obj->Faces[j*3]   == i ||  
					cur_obj->Faces[j*3+1] == i ||  
					cur_obj->Faces[j*3+2] == i) 
				{ 
					AddVector(	vSumx,vSumy,vSumz, 
								pTempNormals[j*3],pTempNormals[j*3+1],pTempNormals[j*3+2], 
								&vSumx,&vSumy,&vSumz);// Add the un-normalized normal of the shared face 
					shared++;								// Increase the number of shared triangles 
				} 
			}       
			 
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out. 
			 DivideVectorByScaler(	vSumx,vSumy,vSumz, 
									&(cur_obj->Normal[i*3]), 
									&(cur_obj->Normal[i*3+1]), 
									&(cur_obj->Normal[i*3+2]), 
									-(float)shared); 
 
			// Normalize the normal for the final vertex normal 
			Normalize(	&(cur_obj->Normal[i*3]), 
						&(cur_obj->Normal[i*3+1]), 
						&(cur_obj->Normal[i*3+2]));	 
 
			vSumx = 0.0;vSumy = 0.0;vSumz = 0.0;			// Reset the sum 
			shared = 0;										// Reset the shared 
		} 
		 
	if (pNormals) free(pNormals); 
	if (pTempNormals) free(pTempNormals); 
	cur_obj=cur_obj->next; 
	}//end for index 
} 

