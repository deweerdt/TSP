#include <stdio.h>

#include "tsp_hash.h"

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

