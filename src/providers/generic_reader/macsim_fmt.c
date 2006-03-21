/*

$Id: macsim_fmt.c,v 1.1 2006-03-21 09:56:59 morvan Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2006 Eric NOULARD and Arnaud MORVAN 

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

Purpose   : Manipulation function of macsim file

-----------------------------------------------------------------------
*/


#include <stdio.h>
#include <string.h>

#include <bb_core.h>
#include <macsim_fmt.h>


/* suppress spaces
*/
char *str_strip (const char *string)
{
   char *strip = NULL;

   if (string)
   {
      strip = malloc (sizeof (*strip) * (strlen (string) + 1));
      if (strip)
      {
         int i, j;
         int ps = 0;

         for (i = 0, j = 0; string[i]; i++)
         {
            if (' '==string[i])
            {
               if (!ps)
               {
                  strip[j] = string[i];
                  ps = 1;
                  j++;
               }
            }
            else
            {
               strip[j] = string[i];
               ps = 0;
               j++;
            }
         }
      }
      else
      {
         fprintf (stderr, "Not enough memory\n");
         exit (EXIT_FAILURE);
      }
   }
   return strip;
}


/*calculate the dimension of the data
*/
uint32_t macsim_dimension_data(char* dimension_var)
{
	uint32_t valeur=0,
	    	 total_valeur=1,
	int 	 i,indice=0;
	    
	char 	 nombre[10];
	
	
	dimension_var=str_strip(dimension_var);
	
	for(i=0;i<strlen(dimension_var);++i)
	{
	
		if('*'==dimension_var[i])
		{
			nombre[indice]='\0';
			valeur=(uint32_t)atoi(nombre);
			total_valeur*=valeur;
			indice=0;
		
		}
		else
		{
			nombre[indice]=dimension_var[i];
			++indice;
		}
	}
	return(total_valeur);
	
}

/* return data size and bb data type
*/
size_t macsim_size_data(char* type_var,E_BB_TYPE_T* type_var_bb)
{
	if (0==strcmp(type_var,DOUBLE_MACSIM))
	{
		*type_var_bb=E_BB_DOUBLE;
		return((size_t)sizeof(double));
	
	}
	
	if (0==strcmp(type_var,ENTIER_MACSIM))
	{
		*type_var_bb=E_BB_INT32;
		return((size_t)sizeof(int32_t));
	
	}
	
	if (0==strcmp(type_var,BOOLEEN_MACSIM))
	{
		*type_var_bb=E_BB_INT8;
		return((size_t)sizeof(int8_t));
	
	}
	
	if (0==strcmp(type_var,CHARACTER_MACSIM))
	{
		*type_var_bb=E_BB_CHAR;
		return((size_t)sizeof(char));
	
	}
	
	return(0);

}

/*open file macsim
*/
FILE * macsim_open(char* nom_fichier_macsim)
{
	FILE * fichier;
	
	fichier=fopen(nom_fichier_macsim,"r");
	if(NULL==fichier)
	{
		fprinf(stderr,"Invalid file: impossible to open file: %s",nom_fichier_macsim);
	}
	return(fichier);
}

/* close file
*/
void macsim_close(FILE* fichier_macsim)
{
	fclose(fichier_macsim);

}

/* read the beginning of the file, create the BB and create the symbol
*/
int macsim_read_header(GenericReader_T* genreader, int32_t justcount)
{
	char* 	      pointeur_buffer, pointeur_buffer_bis;
	char  	      buffer[MAX_BUFFER_MACSIM + 1];
	char  	      nom_var[MAX_NOM_VAR_MACSIM];
	char  	      dimension_var[MAX_DIMENSION_MACSIM];
	char  	      type_var[MAX_TYPE_MACSIM];
	char  	      unite_var[MAX_UNITE_MACSIM];
	E_BB_TYPE_T   type_var_bb;
	int   	      continuer=CONTINUE;
	uint32_t      dimension=0;
	size_t 	      taille=0;
	
		
	if(NULL!=genreader->handler->file)
	{
		
		if(JUSTCOUNT_SIZE_BB==justcount)
		{
			genreader->nbSymbol=0;
			genreader->symbolSize=0;
		}
		else
		{
			rewind(genreader->handler->file);
		}		
		
		while(NULL!=(fgets(buffer,MAX_BUFFER_MACSIM,genreader->handler->file)) && CONTINUE==continuer)
		{
			/*the end of the beginning of file is indicate by the string =============
			*/
			if('='==buffer[0])
			{
				continuer=STOP;
			}
			else
			{
				/*extract the data, all data are separate by ':'
				*/
			
				/*retrieve the symbol name
				*/
				pointeur_buffer=strchr(buffer,':');
				*pointeur_buffer='\0';
				strcpy(nom_var,buffer);
				nom_var=str_strip(nom_var);
				
				/*retrieve the dimension
				*/
				pointeur_buffer_bis=strchr(pointeur_buffeur + 1,':');
				*pointeur_buffer_bis='\0';
				strcpy(dimension_var,pointeur_buffer);
				
				/*calculate the dimension of the variable (si 2*3*5 = 30)
				*/
				dimension=macsim_dimension_data(dimension_var);
				
				/*retrieve the type of the data
				*/
				pointeur_buffer=strchr(pointeur_buffeur_bis + 1,':');
				*pointeur_buffer='\0';
				strcpy(type_var,pointeur_buffer_bis);
				type_var=str_strip(type_var);
				
				/*determine data size and BB type
				*/
				taille=macsim_size_data(type_var,&type_var_bb);
				
				
				/*retrieve the unity
				*/
				strcpy(unite_var,pointeur_buffer + 1);
				unite_var=str_strip(unite_var);
				
				if(JUSTCOUNT_SIZE_BB==justcount)
				{
					/*calculate the memory size of the variable
					*/
					taille*=dimension;
					
					genreader->nbSymbol+=1;
					genreader->symbolSize+=taille;
				}
				else
				{
					genreader->genreader_addvar(nom_var,dimension,taille,type_var_bb,unit_var);
					
				}
				
			}
		
		}

	}
	return(0);
}


/*read the data contain in the file
*/
int macsim_read(GenericReader_T* genreader)
{
	
	char	      caractere_lu;
	char  	      data_var[100];
	int	      i=0,
		      indice_data=0;
	
	if(NULL!=genreader->handler->file)
	{
	
		/* read the column title line
		*/
		while('\0'!=(caractere_lu=(char)fgetc(genreader->handler->file)));
	
	
		while(EOF!=(caractere_lu=(char)fgetc(genreader->handler->file)))
		{
			if ('\0'!=caractere_lu)
			{
			
				if (CARACTERE_TAB!= caractere_lu)
				{
					if (CARACTERE_BLANC!=caractere_lu)
					{
						data_var[i]=caractere_lu;
						++i;
					}
				}
				else
				{
					date_var[i]='\0';
				
					/* add data to the BB
					*/	
					genreader_write(genreader, data_var, indice_data);
					++indice_data;
					i=0;
				}
				
				
			}
			else
			{
				date_var[i]='\0';
				
				/* add data to the BB
				*/	
				genreader_write(genreader, data_var, indice_data);
				
					
				i=0;
				indice_data=0;
			
				/* send the data to the Bb
				*/
				genreader_synchro(genreader);
			}
		}
		
	}
	return(0);
}




/*initialize the handler to read a macsim file
*/
int macsim_createHandler(FmtHandler_T** fmt_handler)
{
	 (*fmt_handler)=(FmtHandler_T*)malloc(sizeof(FmtHandler_T));

	 (*fmt_handler)->open        = &macsim_open;
 	 (*fmt_handler)->close       = &macsim_close;
	 (*fmt_handler)->readHeader  = &macsim_read_header;
	 (*fmt_handler)->readValue   = &macsim_read;
}


