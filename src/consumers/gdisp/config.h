#ifndef _PAGES_CONFIG_H_
#define _PAGES_CONFIG_H_

#include <glib.h>

#include "tsp_consumer.h"

struct conf_data_t
{
  TSP_consumer_symbol_requested_list_t tsp_requested;
  int nb_page;
  gfloat display_frequency;
};

typedef struct conf_data_t conf_data_t;

extern conf_data_t conf_data;

gboolean load_config (gchar *filename, conf_data_t* data);

#endif  /* _PAGES_CONFIG_H_ */
