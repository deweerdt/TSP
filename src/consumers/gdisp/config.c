#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <assert.h>
#include "page.h"
#include "support.h"
#include "page_config.h"

#include "config.h"

#include "math.h"


extern   display_page* pages;



conf_data_t conf_data;

static variable_type pgc_vtype_to_local_vtype(PGC_var_type_t pgc_vtype)
{
  variable_type local_vtype;
  
  switch(pgc_vtype)
    {
    case PGC_DOUBLE :
      local_vtype = VAR_DOUBLE;
      break;
    case PGC_TITLE :
      local_vtype = VAR_TITLE;
      break;
    case PGC_HEXA :
      local_vtype = VAR_HEXA;
      break;
    case PGC_BIN :
      local_vtype = VAR_BIN;
      break;  
    case PGC_STRING :
      local_vtype = VAR_STRING;
      break;
      
    default :
      fprintf(stderr, "Unknown var type : libpage_config version mismatch\n");
      exit(-1);      
    }
  return local_vtype;
}

static widget_type_t pgc_widgettype_to_local_widgettype(PGC_widget_type_t pgc_widgettype)
{
  widget_type_t local_widgettype;
  
  switch(pgc_widgettype)
    {
    case PGC_WIDGET_DRAW :
      local_widgettype = WIDGET_DRAW;
      break;
    case PGC_WIDGET_VIEW :
      local_widgettype = WIDGET_VIEW;
      break;
    default :
      fprintf(stderr, "Unknown widget type : libpage_config version mismatch\n");
      exit(-1);      
    }
  return local_widgettype;
}


gboolean
load_config (gchar *filename, conf_data_t* data)
{
  
  int i = 0;
  PGC_handle_t pgc;
  PGC_page_t page;
  PGC_var_t var;
  PGC_global_t pgc_global;
  variable *new_var;

  data->tsp_requested.len = 0;

  pgc = PGC_open_file(filename);


  if(!pgc)
    {
      fprintf(stderr, "unable to parse file='%s'", filename);
      return FALSE;
    }
  
  /* Allocate room for tsp symbols */
  /* FIXME : 0 symbols ? */
  data->tsp_requested.val =
    (TSP_consumer_symbol_requested_t*)calloc(PGC_get_nb_var(pgc),sizeof(TSP_consumer_symbol_requested_t));

  data->nb_page = PGC_get_nb_page(pgc);
  PGC_get_global(pgc, &pgc_global);
  data->display_frequency = pgc_global.display_frequency;
    
  printf("nb_page = %d\n", PGC_get_nb_page(pgc));
  printf("nb_var = %d\n", PGC_get_nb_var(pgc));
 
  /* Allocate memory for pages */
  pages = (display_page*)g_malloc0(data->nb_page*sizeof(display_page));
  for (i=0; i < data->nb_page ; i++)
    {
    pages[i].variables = g_ptr_array_new();

  }

  i = 0;
  while( PGC_get_next_page(pgc, &page))
    {

      pages[i].position.x = page.x;
      pages[i].position.y = page.y;
      pages[i].position.width = page.width;
      pages[i].position.height = page.height;
      pages[i].position.height = page.height;
      pages[i].default_is_visible = page.is_visible;
      pages[i].no_border = page.no_border;
      pages[i].title = page.title;

      /* if 0 for rows, autocalculation */
      if(page.rows)
	pages[i].rows = page.rows;
      else
	pages[i].rows = sqrt((double)(PGC_get_page_nb_var(pgc, i)));  /* FIXME : Should depend on view | draw */


      while( PGC_get_next_var(pgc, &var) )
	{

	  new_var = g_malloc0(sizeof(variable));
	  if (!new_var)
	    {
	      fprintf(stderr, "Memory allocation failed");
	      return FALSE;
	  }	  
	  new_var->type = pgc_vtype_to_local_vtype(var.type);
	  new_var->widget = NULL;
	  new_var->text = var.name;
	  new_var->legend = var.legend;
	  new_var->widget_type = pgc_widgettype_to_local_widgettype(var.widget_type);
	  new_var->provider_global_index = -1;
	  new_var->period = var.period;
	  new_var->duration = var.duration;
	  g_ptr_array_add(pages[i].variables, new_var);
	  
	  /* FIXME : autres type */
	  if( VAR_DOUBLE == var.type)
	    {
	      /* We do not want to ask twice the same variable */
	      int u; int found = FALSE;
	      for ( u = 0; u < data->tsp_requested.len ; u++)
		{
		  if( !strcmp(data->tsp_requested.val[u].name,var.name))
		    {
		      found = TRUE; break;
		    }
		}
		  
	      if(!found)
		{
		  int current = data->tsp_requested.len++;
		  data->tsp_requested.val[current].name = var.name;
		  data->tsp_requested.val[current].period = var.period;
		  data->tsp_requested.val[current].phase = 0;
		}
	    }
	}

      if( PGC_STATUS_FATAL == PGC_get_last_error(pgc) )
	{
	  fprintf(stderr, "error parsing file='%s'", filename);
	  return FALSE;
	}

      i++;
    }
  
  if( PGC_STATUS_FATAL == PGC_get_last_error(pgc) )
    {
      fprintf(stderr, "error parsing file='%s'", filename);
      return FALSE;
    }

  return TRUE;
}


