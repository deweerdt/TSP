/*

$Id: macsim_fmt.c,v 1.12 2007-03-25 14:17:06 erk Exp $

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
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

/* #include <bb_core.h> */
#include <tsp_rpc.h>
#include <generic_reader.h>
#include <macsim_fmt.h>
#include <tsp_common.h>


/* delete space before and after
*/
char *str_strip (const char *string)
{
   char *strip = NULL;

   if (string)
   {
     int i,j,k=0;

     strip =(char*) malloc (sizeof (char) * (strlen (string) + 1));

     /*find first charactere*/
     for(i=0;i<strlen(string);++i)
     {
       if(!(isspace((int)string[i])))
       {
	 break;
       }
     }
     
     /*find last character*/
     for(j=strlen(string)-1;j>0;--j)
     {
       if(!(isspace((int)string[j])))
       {
	 break;
       }
     }

     /*copy*/
     for(;i<=j;++i)
     {

       strip[k]=string[i];

       ++k;
     }
     strip[k]='\0';
   }
   else
   {
     fprintf (stderr, "Not enough memory\n");
     return NULL;
   }

   return strip;
}

/*delete space before and after in the same data*/
void str_strip_inplace(char* tostrip) {

  char* local = str_strip(tostrip);
  strcpy(tostrip,local);
  free(local);
}


/*calculate the dimension of the data 2*2*3=12
*/
uint32_t macsim_dimension_data(char* dimension_var)
{
	uint32_t valeur=0,
	         total_valeur=1;
	int 	 i,indice=0;
	    
	char 	 nombre[10];
	
	
	str_strip_inplace(dimension_var);
	
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

	nombre[indice]='\0';
	valeur=(uint32_t)atoi(nombre);
	total_valeur*=valeur;
	
	return(total_valeur);
	
}


/* return TSP data type
*/
TSP_datatype_t macsim_type_data(char* type_var)
{ 
  if(NULL!=type_var)
  {

    if (0==strcmp(type_var,DOUBLE_MACSIM))
    {
      return(TSP_TYPE_DOUBLE);
    }
	
    if (0==strcmp(type_var,ENTIER_MACSIM))
    {
      return(TSP_TYPE_INT32);
    }
	
    if (0==strcmp(type_var,BOOLEEN_MACSIM))
    {
      return(TSP_TYPE_UINT8);
    }
	
    if (0==strcmp(type_var,CARACTERE_MACSIM))
    {
      return(TSP_TYPE_CHAR);
    }
  }
  return(TSP_TYPE_UNKNOWN);

}

/*open file macsim
*/
FILE* macsim_open(char* nom_fichier_macsim)
{
	FILE * fichier;
	
	fichier=fopen(nom_fichier_macsim,"r");
	if(NULL==fichier)
	{
		fprintf(stderr,"Invalid file: impossible to open file: %s",nom_fichier_macsim);
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
int32_t macsim_read_header(GenericReader_T* genreader, int32_t justcount)
{
        char* 	      pointeur_buffer;
	char*         pointeur_buffer_bis;
	char  	      buffer[MAX_BUFFER_MACSIM + 1];
	char  	      nom_var[MAX_NOM_VAR_MACSIM];
	char  	      dimension_var[MAX_DIMENSION_MACSIM];
	char  	      type_var[MAX_TYPE_MACSIM];
	char  	      unite_var[MAX_UNITE_MACSIM];
	char  	      buffer_sizestring[10];
	int   	      continuer=CONTINUE;
	uint32_t      dimension=0;
	size_t 	      taille=1;
	uint32_t      indice_symbol=0;
	uint32_t      nb_ext_info;
	uint32_t      EI_chaine=0;
	uint32_t      EI_unit=0;
	TSP_sample_symbol_info_t *ssi=NULL;
	TSP_sample_symbol_extended_info_t *ssei=NULL;
		
	if(NULL!=genreader->handler->file)
	{
		
		if(JUSTCOUNT_SIZE==justcount)
		{
		  /*count nb symbol and max size of symbol*/
		  genreader->nbsymbol=0;
		  genreader->max_size_raw_value=0;
		}
		else
		{
		  ssi=genreader->ssi_list->TSP_sample_symbol_info_list_t_val;
		  ssei=genreader->ssei_list->TSP_sample_symbol_extended_info_list_t_val;

	          /*return to the beginning of the file*/
		  rewind(genreader->handler->file);
		}		
		
		while(NULL!=(fgets(buffer,MAX_BUFFER_MACSIM,genreader->handler->file)) && CONTINUE==continuer)
		{
			/*the end of the beginning of file is indicate by the string =============
			*/
			if('='==buffer[0])
			{
			        /*line '========' indicate the end of the declaration of the symbol*/
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
				str_strip_inplace(nom_var);
				
				/*retrieve the dimension
				*/
				pointeur_buffer_bis=strchr((pointeur_buffer+1),':');
				*pointeur_buffer_bis='\0';
				strcpy(dimension_var,(pointeur_buffer+1));
				
				/*calculate the dimension of the variable (si 2*3*5 = 30)
				*/
				dimension=macsim_dimension_data(dimension_var);

				/*retrieve the type of the data
				*/
				pointeur_buffer=strchr(pointeur_buffer_bis + 1,':');
				*pointeur_buffer='\0';
				strcpy(type_var,(pointeur_buffer_bis+1));
				str_strip_inplace(type_var);

				/*calculate the memory size of the variable
				 */	
				/*determine data size
				 */
				taille=tsp_type_size[macsim_type_data(type_var)];
				taille*=dimension;
				
				/*TYPE_CHAR or TYPE_UCHAR is a  string of  LG_MAX_STRING_MACSIM length */
				if( (TSP_TYPE_CHAR==macsim_type_data(type_var)) || (TSP_TYPE_UCHAR==macsim_type_data(type_var)) )
				{
				  taille*=LG_MAX_STRING_MACSIM;
				  dimension=taille;
				}

				
				/*retrieve the unity
				*/
				strcpy(unite_var,pointeur_buffer + 1);
				str_strip_inplace(unite_var);
				
				if(JUSTCOUNT_SIZE==justcount)
				{
				
					genreader->nbsymbol+=1;

					if(genreader->max_size_raw_value<taille)
					{
					   genreader->max_size_raw_value=taille;
					}
				}
				else
				{
				  /*add symbol to the symbol list and extended info*/
				  EI_unit=0;
				  EI_chaine=0;
				  nb_ext_info=2;

				  /*create ths symbol*/
				  ssi[indice_symbol].name=(char*)calloc(1,strlen(nom_var)+1);
				  strcpy(ssi[indice_symbol].name,nom_var);

				  ssi[indice_symbol].provider_global_index=indice_symbol;

				  ssi[indice_symbol].type=macsim_type_data(type_var);

				  ssi[indice_symbol].dimension=dimension;
				  ssi[indice_symbol].nelem=dimension;

				  ssi[indice_symbol].period=1;

				  ssi[indice_symbol].phase=0;


				  /*test if extended info "unint" exist*/
				  if(0!=strlen(unite_var))
				  {
				    EI_unit=1;
				    ++nb_ext_info;
				  }
				  /*TYPE_CHAR or TYPE_UCHAR is a  string of  LG_MAX_STRING_MACSIM length */
				  if( (TSP_TYPE_CHAR==macsim_type_data(type_var)) || (TSP_TYPE_UCHAR==macsim_type_data(type_var)) )
				  {
				    EI_chaine=1;
				    ++nb_ext_info;

				  }

				  /*create the extended info of the symbol*/
				  TSP_SSEI_initialize(&(ssei[indice_symbol]), indice_symbol,nb_ext_info);

                                  TSP_EI_initialize(&(ssei[indice_symbol].info.TSP_extended_info_list_t_val[0]),"profile",dimension_var);

				  TSP_EI_initialize(&(ssei[indice_symbol].info.TSP_extended_info_list_t_val[1]),"order","1");

				  if(1==EI_unit)
				  {
				    TSP_EI_initialize(&(ssei[indice_symbol].info.TSP_extended_info_list_t_val[2]),"unit",unite_var);
				   
				  }

				  if(1==EI_chaine)
				  {
				    sprintf(buffer_sizestring,"%d",LG_MAX_STRING_MACSIM);
				    /*3-EI_chaine+EI_unit: because the indice can be 2 or 3*/
				    TSP_EI_initialize(&(ssei[indice_symbol].info.TSP_extended_info_list_t_val[3-EI_chaine+EI_unit]),"sizestring",buffer_sizestring);
				    
				  }


				  ++indice_symbol;
					
				}
				
			}
		
		} /*end while*/
		/*read the column header line*/
		while(NULL==strchr(buffer,'\n'))
		{
		  fgets(buffer,MAX_BUFFER_MACSIM,genreader->handler->file);
		}

	}
	return(TSP_STATUS_OK);
}


/*read the data contain in the file
*/
int32_t macsim_read(GenericReader_T* genreader,glu_item_t* item)
{
	char  	      data_var[LG_MAX_STRING_MACSIM];
	uint32_t      indice_data=0,
	              rep=EOF,
	              dimension=0;
	uint8_t       continuer=1;
	

	TSP_sample_symbol_info_list_t  ssi_list;

	ssi_list=*genreader->ssi_list;

	
	if(NULL!=genreader->handler->file)
	{

	  /*TYPE_CHAR or TYPE_UCHAR is a  string of  LG_MAX_STRING_MACSIM length */
	  if( (TSP_TYPE_CHAR==ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].type)
	      || (TSP_TYPE_UCHAR==ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].type) )
	  {
	    item->size=ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].dimension;
	    /*calculate the real dimension for the string symbol*/
	    dimension=ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].dimension/LG_MAX_STRING_MACSIM;
	  }
	  else
	  {
	    item->size=ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].dimension;
	    dimension=ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].dimension;
	      
	  }
	  
	  item->size*=tsp_type_size[ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].type];

	    
	  while( continuer  && 
                (indice_data<dimension))
	  {

	    memset(data_var,'\0',LG_MAX_STRING_MACSIM);

	    /*read one data of the file*/
	    rep=read_data_file(genreader->handler->file,data_var);
              
	    if(EOF!=rep)
	    {
	      /*end of line*/
	      if(END_SAMPLE_SET==rep)
	      {
		continuer=0;
	      }

	      /*load retrieve data in the raw value*/
	      switch(ssi_list.TSP_sample_symbol_info_list_t_val[item->provider_global_index].type) 
	      {

	        case TSP_TYPE_DOUBLE :
		  load_double(data_var,item->raw_value,indice_data);
		  break;
				       
	        case TSP_TYPE_FLOAT :
		  load_float(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_INT8 :
		  load_int8(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_INT16:
		  load_int16(data_var,item->raw_value,indice_data);
		  break;

	        case TSP_TYPE_INT32 :
		  load_int32(data_var,item->raw_value,indice_data);
		  break;
      
	        case TSP_TYPE_INT64 :
		  load_int64(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_UINT8:
		  load_uint8(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_UINT16:
		  load_uint16(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_UINT32:
		  load_uint32(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_UINT64:
		  load_uint64(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_CHAR:
		  load_char(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_UCHAR:
		  load_uchar(data_var,item->raw_value,indice_data);
		  break;
    
	        case TSP_TYPE_RAW:
		  load_type_raw(data_var,item->raw_value,indice_data);
		  break;
    
	        default:
		  break;
	      }

	      ++indice_data;
	    
	    }
	    else
	    {
	      continuer=0;
	    }

	  }
   
	}
         return(rep);
}




/*initialize the handler to read a macsim file
*/
int32_t macsim_createHandler(FmtHandler_T** fmt_handler)
{
	 (*fmt_handler)=(FmtHandler_T*)malloc(sizeof(FmtHandler_T));

	 (*fmt_handler)->open_file        = &macsim_open;
 	 (*fmt_handler)->close_file       = &macsim_close;
	 (*fmt_handler)->read_header  = &macsim_read_header;
	 (*fmt_handler)->read_value   = &macsim_read;

	 return TSP_STATUS_OK;
}


int32_t read_data_file(FILE *fic,char *data_var)
{
	char	      caractere_lu;
	uint32_t      i=0;
	uint8_t       begin_data=0;
	             
	while(EOF!=(caractere_lu=(char)fgetc(fic)))
	{

	  if(!begin_data)
	  {
	    if(!isspace((int)caractere_lu))
	    {
	      begin_data=1;
	      data_var[i]=caractere_lu;
	      ++i;
	    }
	      
	  }
	  else
	  {
	    if(isspace((int)caractere_lu))
	    {
	      if(CARACTERE_BLANC!=caractere_lu)
	      {
		if(CARACTERE_TAB==caractere_lu)
		{
		  data_var[i]='\0'; 
		  /*end of a line*/
		  return END_SAMPLE_STREAM;
		}
		else
		{
		  data_var[i]='\0';
		  /*end of one data*/
		  return END_SAMPLE_SET;
		}
	      }
	    }
	    data_var[i]=caractere_lu;
	    ++i;
	  }
	}
	return(EOF);
}

int32_t load_double(char* data_var,void* raw_value,const uint32_t indice_data)
{
  double *tab;
  char   *rep;

  tab=(double*)raw_value;
  tab[indice_data]=strtod(data_var,&rep);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion double for <%s>\n",data_var));
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  return TSP_STATUS_OK;
}

int32_t load_float(char* data_var,void* raw_value,const uint32_t indice_data)
{
  float *tab;
  char  *rep;

  tab=(float*)raw_value;

  /*FIXME: use strtod because can't use strtof*/
  tab[indice_data]=(float)strtod(data_var,&rep);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion float\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  return TSP_STATUS_OK;

}

int32_t load_int8(char* data_var,void* raw_value,const uint32_t indice_data)
{
  int8_t *tab;
  char* rep;

  tab=(int8_t*)raw_value;
  tab[indice_data]=(int8_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion int8\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  return TSP_STATUS_OK;

}

int32_t load_int16(char* data_var,void* raw_value,const uint32_t indice_data)
{
  int16_t *tab;
  char* rep;

  tab=(int16_t*)raw_value;
 
  tab[indice_data]=(int16_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion int16\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  return TSP_STATUS_OK;

}

int32_t load_int32(char* data_var,void* raw_value,const uint32_t indice_data)
{
  int32_t *tab;
  char* rep;

  tab=(int32_t*)raw_value;
  
  tab[indice_data]=(int32_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion int32\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }
  return TSP_STATUS_OK;

}

int32_t load_int64(char* data_var,void* raw_value,const uint32_t indice_data)
{
  int64_t *tab;
  char* rep;

  tab=(int64_t*)raw_value;
  
  tab[indice_data]=(int64_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion int64\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }
  return TSP_STATUS_OK;

}

int32_t load_uint8(char* data_var,void* raw_value,const uint32_t indice_data)
{
  uint8_t *tab;
  char* rep;

  tab=(uint8_t*)raw_value;
  
  tab[indice_data]=(uint8_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion uint8\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }
  return TSP_STATUS_OK;

}

int32_t load_uint16(char* data_var,void* raw_value,const uint32_t indice_data)
{
  uint16_t *tab;
  char* rep;

  tab=(uint16_t*)raw_value;
  
  tab[indice_data]=(uint16_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion uint16\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }
  return TSP_STATUS_OK;

}

int32_t load_uint32(char* data_var,void* raw_value,const uint32_t indice_data)
{
  uint32_t *tab;
  char* rep;

  tab=(uint32_t*)raw_value;
  
  tab[indice_data]=(uint32_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion uint32\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }
  return TSP_STATUS_OK;

}

int32_t load_uint64(char* data_var,void* raw_value,const uint32_t indice_data)
{
  uint64_t *tab;
  char* rep;

  tab=(uint64_t*)raw_value;
  
  tab[indice_data]=(uint64_t)strtol(data_var,&rep, 10);

  if(data_var==rep || '\0'!=*rep)
  {
    STRACE_ERROR(("Error conversion uint64\n"));
    return TSP_STATUS_ERROR_UNKNOWN;
  }
  return TSP_STATUS_OK;

}

int32_t load_char(char* data_var,void* raw_value,const uint32_t indice_data)
{
  char *tab;

  tab=(char*)raw_value;
 
  memcpy(&(tab[indice_data*LG_MAX_STRING_MACSIM]),data_var,LG_MAX_STRING_MACSIM);
 /* memcpy(&(tab[indice_data]),data_var,strlen(data_var)+1);*/

  return TSP_STATUS_OK;

}

int32_t load_uchar(char* data_var,void* raw_value,const uint32_t indice_data)
{
  unsigned char *tab;
  
  tab=(unsigned char *)raw_value;
  
/*  memcpy(&(tab[indice_data]),data_var,strlen(data_var)+1);*/
  memcpy(&(tab[indice_data*LG_MAX_STRING_MACSIM]),data_var,LG_MAX_STRING_MACSIM);

  return TSP_STATUS_OK;

}

int32_t load_type_raw(char* data_var,void* raw_value,const uint32_t indice_data)
{
  uint8_t *tab;

  tab=(uint8_t*)raw_value;
  memcpy(&(tab[indice_data]),data_var,strlen(data_var));

  return TSP_STATUS_OK;

}
