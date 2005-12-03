/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libutil/tsp_hash.c,v 1.3 2005-12-03 12:45:07 esteban Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyleft (c) 2005 Robert PAGNOT 

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
Maintainer : tsp@astrium.eads.net
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Hash Table implementation, performance oriented :
               - look-up time depends only on string length
	       - memory used depends on "prefix" similarity of table
	         contents

-----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <unistd.h>

#include <tsp_hash.h>

#define HASH_INDEX(hash, ch)  ( \
          ((int)(ch) >= (hash)->first && (int)(ch) <= (hash)->last) ? \
               (int)(ch) - (hash)->first : -1 )

#define HASH_LENGTH(hash) ((hash)->last - (hash)->first + 1)


#define HASH_MEMPOOL_NB 10000     /*--- number of tables per realloc---*/
#define HASH_MEMPOOL_LENGTH(hash)   /*--- length of each memory pool ---*/ \
                   (sizeof(void*) + HASH_LENGTH(hash)*sizeof(int))

#define HASH_MEMPOOL_GET(hash, mpi)  /*--- get hash table from memory pool ---*/ \
             (hash_table_t*)((hash)->memPool + (mpi)*(HASH_MEMPOOL_LENGTH(hash)))

/* FIXME : this string is only for debugging purposes, no reentrant
   may be put it to hash_t structure ? */
char *_string;


int hash_calloc(hash_t *hash)
{
  int  mpi;

  if(hash->nb_tables % HASH_MEMPOOL_NB == 0)
    {
      hash->memPool = (char*) realloc(hash->memPool, HASH_MEMPOOL_LENGTH(hash) *
				      (hash->nb_tables + HASH_MEMPOOL_NB) );
      assert(hash->memPool);

      bzero(hash->memPool + (hash->nb_tables * HASH_MEMPOOL_LENGTH(hash)),
	    HASH_MEMPOOL_NB * HASH_MEMPOOL_LENGTH(hash));

      /*
	printf("\trealloc %d bytes from table %d : 0x%p\n", HASH_MEMPOOL_LENGTH(hash) *
	(hash->nb_tables + HASH_MEMPOOL_NB), hash->nb_tables, hash->memPool);
	printf("\t\tbzero %d bytes from 0x%p\n", HASH_MEMPOOL_NB * HASH_MEMPOOL_LENGTH(hash),hash->memPool + (hash->nb_tables * HASH_MEMPOOL_LENGTH(hash)));
      */
    }

  mpi = hash->nb_tables;
  hash->nb_tables++;

  return mpi;
}

hash_t* hash_open(int first, int last)
{
  hash_t        *hash;

  hash = (hash_t*) calloc(1, sizeof(hash_t));
  assert(hash);

  hash->memPool = NULL;

  hash->nb_tables = 0;
  hash->nb_strings = 0; 
  hash->nb_chars = 0; 

  hash->first = first;
  hash->last = last;

  /*--- index 0 is reserved for master structure ---*/
  hash->table = hash_calloc(hash);

  return hash;
}


int hash_append_recurse(hash_t *hash, int mpi, char *string, void *id)
{
  hash_table_t *table;
  int new;

  table = HASH_MEMPOOL_GET(hash, mpi);
  if(*string == 0)
    {
      table->id = id;
      /*printf("Appended %s = %d\n", _string, (int)table->id);*/
      return 0;
    }
  else
    {
      int idx = HASH_INDEX(hash, *string);
      
      if(idx < 0)
	{
	  printf("<%c> is out of scope for <%s> = %d\n", *string, _string, (int)id);
	  return -1;
	}
	  
      if(table->next[idx] == 0)
	{
	  /*--- Request new table---*/
	  new = hash_calloc(hash);

	  /*--- if memPool has moved, table pointer also moved, get it again ---*/
	  table = HASH_MEMPOOL_GET(hash, mpi);
	  table->next[idx] = new;
	}

      hash->nb_chars++;
      string++;

      return  hash_append_recurse(hash, table->next[idx], string, id);
	
    }
}

int hash_append(hash_t *hash, char *string, void *id)
{
  if(!hash || !string)
    return -1;

  _string = string;
  hash->nb_strings++; 
  return hash_append_recurse(hash, hash->table, string, id);
}


void *hash_get_recurse(hash_t *hash, int mpi, char *string)
{
  int idx;
  hash_table_t *table = HASH_MEMPOOL_GET(hash, mpi);

  if(!table || !string)
    return NULL;

  if(*string == 0)
    {
      /*printf("Got %s = %d\n", _string, (int)table->id);*/
      return table->id;
    }

  idx = HASH_INDEX(hash, *string);      
  if(idx < 0)
    {
      return NULL;
    }

  if(table->next[idx] > 0)
    {
      return hash_get_recurse(hash,
			      table->next[idx],
			      string+1);
    }
  return NULL;
}


void *hash_get(hash_t *hash, char *string)
{
  if(!hash || !string)
    return NULL;

  _string = string;
  return hash_get_recurse(hash, hash->table, string);
}


int hash_close(hash_t *hash)
{
  if(!hash)
    return -1;

  free(hash->memPool);
  hash->memPool = NULL;

  hash->nb_tables = 0;
  hash->nb_strings = 0; 
  hash->nb_chars = 0; 

  hash->first = -1;
  hash->last = -1;

  hash->table = 0;

  free(hash);

  return 0;
}

/*------------------------------------------------------------------------*/

void hash_dump_recurse(hash_t *hash, int mpi, char *string)
{
  int i, idx;
  hash_table_t *table = HASH_MEMPOOL_GET(hash, mpi);

  if(!table || !string)
    return;

  /**/
  if(table->id != 0)
    printf("%s = %d\n", _string, (int)table->id);
  /**/

  for(i=hash->first; i<=hash->last; i++)
    {
      idx = HASH_INDEX(hash, i);
      
      if(idx < 0)
	{
	  printf("<%c> is out of scope for <%s>\n", *string, _string);
	  return;
	}

      if(table->next[idx] > 0)
	{
	  string[0] = i;
	  string[1] = 0;
	  hash_dump_recurse(hash,
			    table->next[idx],
			    string+1);
	}
    }
} 
void hash_dump(hash_t *hash, int level)
{
  char string[1024];
  int i,j, idx;
  hash_table_t *table;

  if(!hash)
    return;

  if(level >= 2)
    {
      bzero(string, 1023);
      _string = string;
      hash_dump_recurse(hash,
			hash->table,
			string);
    }

  if(level >= 2)
    {
      for(i=0; i<hash->nb_tables; i++)
	{
	  table = HASH_MEMPOOL_GET(hash, i);
	  printf("(%d) : %p => ", i, table->id);
	  for(j = hash->first; j <= hash->last; j++)
	    {
	      idx = HASH_INDEX(hash, j);
	      if(table->next[idx] > 0)
		printf("%c(%d) ", j, table->next[idx]);
	    }
	  printf("\n");
	}
    }


     printf("\n%d strings set on %d characters :\n", hash->nb_strings, HASH_LENGTH(hash));
     for(i = hash->first; i <= hash->last; i++)
       printf("%c",i);
     printf("\n\n");
      
     printf("%d tables of %d bytes each : memory = %.3f Mbytes\n", hash->nb_tables, HASH_MEMPOOL_LENGTH(hash), (double) hash->nb_tables * HASH_MEMPOOL_LENGTH(hash) / (1024*1024));
     printf("%d total characters : ", hash->nb_chars);
     printf("density = %.6f\n\n", (double) hash->nb_tables / hash->nb_chars);


}

#ifdef MAIN

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct {
  char *string;
  unsigned long id;
} strcmp_t;

strcmp_t _strcmp_db[20000];
int _strcmp_idx = 0;

void strcmp_put(char *string, unsigned long id)
{
  _strcmp_db[_strcmp_idx].string = strdup(string);
  _strcmp_db[_strcmp_idx].id = id;
  _strcmp_idx++;
}

int strcmp_get(char *string)
{
  int i;
  for(i=0; i<_strcmp_idx; i++)
    if(strcmp(string, _strcmp_db[i].string) == 0)
       return _strcmp_db[i].id;

  return 0;
}

int
main (int argc, char** argv) {
  extern char*           optarg;
  int                  opt;
  int                  opterr = FALSE;
  FILE                *fp;
  char                 *fileName = NULL;
  int                  dump = 0;
  int                  use_strcmp = FALSE;
  hash_t             *hash;
  int                 i;
  char                string[1024], *p;

  /* Check line command arguments */

  while ((opt = getopt(argc, argv, "f:d:ch")) != EOF) {
    switch (opt) {
    case 'f': fileName = strdup(optarg); break;
    case 'd': dump = atoi(optarg); break;
    case 'c': use_strcmp = TRUE; break;
    default : opterr  = TRUE;   break;
    }
  }

  if (opterr == TRUE || fileName == NULL)
    {
      printf("USAGE : %s -f filename [-d level] [-c]\n", argv[0]);
      printf("\tlevel = 0 : dump only table statistics (default)\n");
      printf("\tlevel = 1 : dump also tables\n");
      printf("\tlevel = 2 : dump also strings and pointers\n");
      printf("\t-c        : use also strcmp to build/lookup table on filename\n");
      exit(-1);
    }

  /*--- Build HASH table from file ---*/
  hash = hash_open('.', 'z');

  fp = fopen(fileName, "r");

  i = 1;
  while(!feof(fp))
    {
      *string = 0;
      fgets(string, 1023, fp);
      if(p=strchr(string,' ')) *p = 0;
      if(p=strchr(string,'\t')) *p = 0;
      if(p=strchr(string,'\n')) *p = 0;
      hash_append(hash, string, (void*)i);
      i++;
    }

  fclose(fp);

  /*--- Dump HASH table ---*/
  hash_dump(hash, dump);


  /*--- Get all strings of file from HASH table ---*/
  fp = fopen(fileName, "r");
  i = 1;
  while(!feof(fp))
    {
      *string = 0;
      fgets(string, 1023, fp);
      if(p=strchr(string,' ')) *p = 0;
      if(p=strchr(string,'\t')) *p = 0;
      if(p=strchr(string,'\n')) *p = 0;
      if((int)hash_get(hash, string) != i) printf("Error for %s, id is not %d\n", string, i);
      i++;
    }

  fclose(fp);

  hash_close(hash);

  /*--- Build and search from file using strcmp method ---*/
  if(use_strcmp)
    {
      fp = fopen(fileName, "r");
      i = 1;
      while(!feof(fp))
	{
	  *string = 0;
	  fgets(string, 1023, fp);
	  if(p=strchr(string,' ')) *p = 0;
	  if(p=strchr(string,'\t')) *p = 0;
	  if(p=strchr(string,'\n')) *p = 0;
	  strcmp_put(string, i);
	  i++;
	}
      fclose(fp);

      fp = fopen(fileName, "r");
      i = 1;
      while(!feof(fp))
	{
	  *string = 0;
	  fgets(string, 1023, fp);
	  if(p=strchr(string,' ')) *p = 0;
	  if(p=strchr(string,'\t')) *p = 0;
	  if(p=strchr(string,'\n')) *p = 0;
	  if(strcmp_get(string) != i) printf("Error for %s, id is not %d\n", string, i);
	  i++;
	}
      fclose(fp);
    }
  return 0;
}

#endif
