#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/time.h>
#include "alias.h"


#define	DBG	if (0)		/* Turn 0 to 1 to activate debug traces */


#define DIFF_TIME(_begin,_end,_res)			\
  if (_end.tv_usec < _begin.tv_usec) {			\
    _res.tv_sec  = _end.tv_sec - _begin.tv_sec - 1;	\
    _res.tv_usec = 1e6 + _end.tv_usec - _begin.tv_usec;	\
  }							\
  else {						\
    _res.tv_sec  = _end.tv_sec - _begin.tv_sec;		\
    _res.tv_usec = _end.tv_usec - _begin.tv_usec;	\
  }


typedef struct {
  char	*item;
  char	*label;
} alias_T;


int	alias_used;



/**************************************************
* Comparaison de 2 aliases                        *
**************************************************/
static int compare_aliases(const void *alias1, const void *alias2)
{
  return strcmp(((alias_T *)alias1)->item, ((alias_T *)alias2)->item);
}


/**************************************************
* Lecture du fichier des aliases                  *
**************************************************/
void init_aliases(int benchmark, int n_variables, char **namev)
{
  static alias_T	alias[ALIAS_MAX_NB];
  alias_T		name, *aliasFound;
  FILE			*fp;
  static char		fileName[MAXPATHLEN+MAXNAMELEN];
  char			buf[256], *item, *label, *end;
  int			aliasNb = 0, i;
  struct timeval	time1[4], time2;


  if (benchmark)
    if (gettimeofday(&time1[0], NULL) < 0) {
      printf("INFO init_aliases: gettimeofday(0) failed\n");
      benchmark = 0;
    }

  if ((item = getenv("BENCH_DATA")) == NULL) {
    printf("WARNING init_aliases: BENCH_DATA unknown\n");
    alias_used = 0;
    return;
  }
  strcpy(fileName, item);
  strcat(fileName, "/");
  strcat(fileName, ALIAS_FILE);

  if ((fp = fopen(fileName, "rt")) == NULL) {
    printf("WARNING init_aliases: aliases file \"%s\" not found\n", fileName);
    alias_used = 0;
    return;
  }
  DBG printf("INFO aliases file is \"%s\"\n", fileName);

  /* Get the labels */
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    if ((item = strtok(buf, " \t\n")) != NULL) {
      label = item + strlen(item) + 1;
      while ((*label == ' ') || (*label == '\t'))
        label++;
      end = label + strlen(label) - 1;
      while ((*end == ' ') || (*end == '\t') || (*end =='\n'))
        end--;
      *(end+1) = '\0';
      if (label <= end) {
        DBG printf("\"%s\" <-> \"%s\"\n", item, label);
        alias[aliasNb].item  = strdup(item);
        alias[aliasNb].label = strdup(label);
        if (++aliasNb == ALIAS_MAX_NB) {
          printf("INFO init_aliases: ALIAS_MAX_NB reached !!!\n");
          /* Don't manage any further label */
          break;
        }
      }
      else
        printf("WARNING init_aliases: \"%s\" has an unrecognized label !!!\n",
		item);
    }
  }


  /* Quick-sort the items */
  if (benchmark) {
    if (gettimeofday(&time1[1], NULL) < 0) {
      printf("INFO init_aliases: gettimeofday(1) failed\n");
      benchmark = 0;
    }

    qsort(alias, aliasNb, sizeof(alias_T), compare_aliases);

    if (gettimeofday(&time1[2], NULL) < 0) {
      printf("INFO init_aliases: gettimeofday(2) failed\n");
      benchmark = 0;
    }
  }
  else
    qsort(alias, aliasNb, sizeof(alias_T), compare_aliases);


  /* Match var names to their labels */
  for (i=0; i<n_variables; i++) {
    name.item = namev[i];
    /* Try to find 'name' in 'alias' table -> 'aliasFound' */
    if ((aliasFound = bsearch
        (&name, alias, aliasNb, sizeof(alias_T), compare_aliases)) != NULL) {
      DBG printf("\"%s\" <-> \"%s\"\n", aliasFound->item, aliasFound->label);
      if (namev[i])
        free(namev[i]);
      namev[i] = strdup(aliasFound->label);
      assert(namev[i]);
    }
    /*
    else
      printf("\"%s\" is not declared in aliases file !!!\n", namev[i]);
    */
  }

  /* Free allocated resources */
  for (i=0; i<aliasNb; i++) {
    free(alias[i].item);
    free(alias[i].label);
  }
  fclose(fp);

  if (benchmark) {
    if (gettimeofday(&time1[3], NULL) < 0)
      printf("INFO init_aliases: gettimeofday(3) failed\n");
    DIFF_TIME(time1[0], time1[3], time2);
    printf("INFO init_aliases: %ld,%06ld sec for %d variables and %d aliases\n",
      time2.tv_sec, time2.tv_usec, n_variables, aliasNb);

    DIFF_TIME(time1[0], time1[1], time2);
    printf("                  (%ld,%06ld + ", time2.tv_sec, time2.tv_usec);
    DIFF_TIME(time1[1], time1[2], time2);
    printf("%ld,%06ld + ", time2.tv_sec, time2.tv_usec);
    DIFF_TIME(time1[2], time1[3], time2);
    printf("%ld,%06ld)\n", time2.tv_sec, time2.tv_usec);
  }
}
