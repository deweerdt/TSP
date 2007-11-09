 

#include <libxml/parser.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "page_config.h"


/* Don't forget to update PGC_var_type_t  AND var_type_str */
static xmlChar* X_var_type_str[] = { BAD_CAST "DOUBLE", BAD_CAST "TITLE", BAD_CAST "HEXA", BAD_CAST "BIN", BAD_CAST "STRING", NULL};
static xmlChar* X_widget_type_str[] = { BAD_CAST "view", BAD_CAST "draw" ,NULL};

const xmlChar* PGC_LABEL_DOC = BAD_CAST "page_config";
const xmlChar* PGC_LABEL_TRUE = BAD_CAST "true";
const xmlChar* PGC_LABEL_FALSE = BAD_CAST "false";
const xmlChar* PGC_LABEL_TITLE = BAD_CAST "title";
const xmlChar* PGC_LABEL_X = BAD_CAST "x";
const xmlChar* PGC_LABEL_Y = BAD_CAST "y";
const xmlChar* PGC_LABEL_WIDTH = BAD_CAST "width";
const xmlChar* PGC_LABEL_HEIGHT = BAD_CAST "height";
const xmlChar* PGC_LABEL_VISIBLE = BAD_CAST "visible";
const xmlChar* PGC_LABEL_NAME = BAD_CAST "name";
const xmlChar* PGC_LABEL_TYPE = BAD_CAST "type";
const xmlChar* PGC_LABEL_LEGEND = BAD_CAST "legend";
const xmlChar* PGC_LABEL_PERIOD = BAD_CAST "period";
const xmlChar* PGC_LABEL_DISPLAY_FREQUENCY = BAD_CAST "display_frequency";
const xmlChar* PGC_LABEL_WIDGET = BAD_CAST "widget";
const xmlChar* PGC_LABEL_ROWS = BAD_CAST "rows";
const xmlChar* PGC_LABEL_NO_BORDER = BAD_CAST "no_border";
const xmlChar* PGC_LABEL_DURATION = BAD_CAST "duration";


#define UTF_TO_CHAR (char*)

struct PGC_instance_t
{
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr page_node;
  xmlNodePtr var_node;
  PGC_error_t last_error;
  float display_frequency;

  int global_period;
  int page_period;

  float global_duration;
  float page_duration;

  PGC_widget_type_t global_widget_type;
  PGC_widget_type_t page_widget_type;

  int global_is_visible;

  int global_no_border;

};

#define PGC_XMLGETPROP_INT(node, name, value, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  value = atoi(UTF_TO_CHAR buf);		\
	  xmlFree(buf); \
        } \
      else \
	{ \
	  fprintf(stderr, "unable to find propertie '%s'\n", name); \
	  return ret; \
	} \
}

#define PGC_XMLGETPROP_INT_OPT(node, name, value, default) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  value = atoi(UTF_TO_CHAR buf);		\
	  xmlFree(buf); \
        } \
      else \
	{ \
          value = default; \
	} \
}



#define PGC_XMLGETPROP_DOUBLE(node, name, value, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  value = atof(UTF_TO_CHAR buf);		\
	  xmlFree(buf); \
        } \
      else \
	{ \
	  fprintf(stderr, "unable to find propertie '%s'\n", name); \
	  return ret; \
	} \
}


#define PGC_XMLGETPROP_DOUBLE_OPT(node, name, value, default) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  value = atof(UTF_TO_CHAR buf);		\
	  xmlFree(buf); \
        } \
      else \
	{ \
          value = default; \
	} \
}

#define PGC_XMLGETPROP_STRING(node, name, value, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  value = strdup(UTF_TO_CHAR buf); \
          xmlFree(buf); \
        } \
      else \
	{ \
	  fprintf(stderr, "unable to find propertie '%s'\n", name); \
	  return ret; \
	} \
}

#define PGC_XMLGETPROP_STRING_OPT(node, name, value, default) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  value = strdup(UTF_TO_CHAR buf); \
          xmlFree(buf); \
        } \
      else \
        { \
          value = default; \
        } \
}

#define PGC_XMLGETPROP_BOOL(node, name, value, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  if(!xmlStrcasecmp(buf, PGC_LABEL_TRUE)) \
	      { \
		value = PGC_TRUE; \
		xmlFree(buf); \
	      } \
	  else if(!xmlStrcasecmp(buf, PGC_LABEL_FALSE)) \
	     { \
	       value = PGC_FALSE; \
	       xmlFree(buf); \
	     } \
	  else \
	     { \
	       fprintf(stderr, "The '%s' propertie must be 'true' or 'false'\n", name); \
	       return ret; \
	     } \
        } \
      else \
	{ \
	  fprintf(stderr, "unable to find propertie '%s'\n", name); \
	  return ret; \
	} \
}

#define PGC_XMLGETPROP_BOOL_OPT(node, name, value, default, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
	  if(!xmlStrcasecmp(buf, PGC_LABEL_TRUE)) \
	      { \
		value = PGC_TRUE; \
		xmlFree(buf); \
	      } \
	  else if(!xmlStrcasecmp(buf, PGC_LABEL_FALSE)) \
	     { \
	       value = PGC_FALSE; \
	       xmlFree(buf); \
	     } \
	  else \
	     { \
	       fprintf(stderr, "The '%s' propertie must be 'true' or 'false'\n", name); \
	       return ret; \
	     } \
        } \
      else \
	{ \
          value = default; \
	} \
}


#define PGC_XMLGETPROP_VARTYPE(node, name, value, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
           if(!var_type_val(buf,&(value))) \
           { \
              fprintf(stderr, "The '%s' variable type is unknown\n", buf); \
	      return ret; \
           } \
        } \
      else \
	{ \
	  fprintf(stderr, "unable to find propertie '%s'\n", name); \
	  return ret; \
	} \
}

#define PGC_XMLGETPROP_WIDGETTYPE(node, name, value, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
           if(!widget_type_val(buf,&(value))) \
           { \
              fprintf(stderr, "The '%s' widget type is unknown\n", buf); \
	      return ret; \
           } \
        } \
      else \
	{ \
	  fprintf(stderr, "unable to find propertie '%s'\n", name); \
	  return ret; \
	} \
}

#define PGC_XMLGETPROP_WIDGETTYPE_OPT(node, name, value, default, ret) \
{ \
      xmlChar* buf = xmlGetProp(node, name); \
      if(buf) \
	{ \
           if(!widget_type_val(buf,&(value))) \
           { \
              fprintf(stderr, "The '%s' widget type is unknown\n", buf); \
	      return ret; \
           } \
        } \
      else \
	{ \
           value = default; \
	} \
}


static int var_type_val (xmlChar *str, PGC_var_type_t* type  ) {
  int i;
  for (i=0; X_var_type_str[i] != NULL; i++) {
    if (!xmlStrcasecmp(str, X_var_type_str[i]))
      {
	*type = (PGC_var_type_t)i;
	return PGC_TRUE;
      }
  }
  return PGC_FALSE;
}

static int widget_type_val (xmlChar *str, PGC_widget_type_t* type  ) {
  int i;
  for (i=0; X_widget_type_str[i] != NULL; i++) {
    if (!xmlStrcasecmp(str, X_widget_type_str[i]))
      {
	*type = (PGC_widget_type_t)i;
	return PGC_TRUE;
      }
  }
  return PGC_FALSE;
}




/*void PGC_close(PGC_handle_t* h)
{
  
}*/

PGC_handle_t PGC_open_file(char* filename)
{
  PGC_instance_t* h;
  h = (PGC_instance_t*)calloc(1, sizeof(PGC_instance_t));
  if(h)
    {
      h->last_error = PGC_STATUS_OK;
      h->doc = xmlParseFile(filename);     
      if (h->doc) 
	{
	  /* root might not be h->doc->children */
	  h->root = xmlDocGetRootElement(h->doc);
	  /* Try to avoid simple error cases */
	  if (h->root &&
	      h->root->name &&
	      !xmlStrcmp(h->root->name, PGC_LABEL_DOC) ) 
	    {
	      /* Read properties */
	      PGC_XMLGETPROP_INT(h->root, PGC_LABEL_PERIOD, h->global_period, NULL);
	      PGC_XMLGETPROP_DOUBLE(h->root, PGC_LABEL_DISPLAY_FREQUENCY, h->display_frequency, NULL);	  
	      PGC_XMLGETPROP_DOUBLE(h->root, PGC_LABEL_DURATION, h->global_duration, NULL);	  
	      PGC_XMLGETPROP_WIDGETTYPE(h->root, PGC_LABEL_WIDGET, h->global_widget_type, NULL);
	      PGC_XMLGETPROP_BOOL(h->root, PGC_LABEL_VISIBLE, h->global_is_visible, NULL);
	      PGC_XMLGETPROP_BOOL(h->root, PGC_LABEL_NO_BORDER, h->global_no_border, NULL);
	      /* Set first page */
	      h->page_node = h->root->children;
	    }
	  else
	    {
	      fprintf(stderr, "File %s data is corrupted, or not a %s file, unable to parse\n", PGC_LABEL_DOC, filename);
	      xmlFreeDoc(h->doc);
	      free(h); h = NULL;
	  }
	}
      else
	{
	  fprintf(stderr, "Cannot allocate a new xml document\n");
	  free(h); h = NULL;
	}
    }
  else
    {
      fprintf(stderr, "memory allocation failed\n");
    }

  return h;
}

int PGC_get_global(PGC_handle_t h, PGC_global_t* global)
{
  global->display_frequency = h->display_frequency;
  return 0;
}

int PGC_get_page_nb_var(PGC_handle_t h, int page)
{
  xmlNodePtr page_node = h->root->children;
  int current_page = 0;
  int nb_var = 0;

  h->last_error = PGC_STATUS_OK;
    
  /* Iterate till next page */
  while( page_node  )
    {
      if(!xmlIsBlankNode(page_node))
	{
	  if(current_page == page)
	    {
	      xmlNodePtr var_node = page_node->children;
	      while( var_node )
		{
		  if(!xmlIsBlankNode(var_node))
		    {
		      nb_var++;
		    }
		  var_node = var_node->next;
		}
	      break;
	    }
	  current_page++;
	}
      page_node = page_node->next;
    }
  return nb_var;

}

int PGC_get_nb_page(PGC_handle_t h)
{
  
  xmlNodePtr page_node = h->root->children;
  int nb_page = 0;

  h->last_error = PGC_STATUS_OK;
    
  /* Iterate till next page */
  while( page_node  )
    {
      if(!xmlIsBlankNode(page_node))
	{
	  nb_page++;
	}
      page_node = page_node->next;
    }
  return nb_page;
}

int PGC_get_nb_var(PGC_handle_t h)
{

  xmlNodePtr page_node = h->root->children;
  xmlNodePtr var_node = NULL;
  int nb_var = 0;

  h->last_error = PGC_STATUS_OK;

  /* Iterate till next page */
  while( page_node  )
    {
      if(!xmlIsBlankNode(page_node))
	{
	  var_node = page_node->children;
	  while( var_node )
	    {
	      if(!xmlIsBlankNode(var_node))
		{
		  nb_var++;
		}
	      var_node = var_node->next;
	    }
	}
      page_node = page_node->next;
    }
  return nb_var;
}

int PGC_get_next_page(PGC_handle_t h, PGC_page_t* page)
{ 

   h->last_error = PGC_STATUS_FATAL;
   
  /* Iterate till next page */
  while( h->page_node && xmlIsBlankNode(h->page_node) )
    {
      h->page_node = h->page_node->next;
    }

  if(h->page_node)
    {
      PGC_XMLGETPROP_STRING(h->page_node, PGC_LABEL_TITLE, page->title, PGC_FALSE);
      PGC_XMLGETPROP_INT(h->page_node, PGC_LABEL_X, page->x, PGC_FALSE);
      PGC_XMLGETPROP_INT(h->page_node, PGC_LABEL_Y, page->y, PGC_FALSE);
      PGC_XMLGETPROP_INT(h->page_node, PGC_LABEL_WIDTH, page->width, PGC_FALSE);
      PGC_XMLGETPROP_INT(h->page_node, PGC_LABEL_HEIGHT, page->height, PGC_FALSE);	 
      PGC_XMLGETPROP_BOOL_OPT(h->page_node, PGC_LABEL_VISIBLE, page->is_visible, h->global_is_visible, PGC_FALSE);	
      /* global period is used if page period is not found */
      PGC_XMLGETPROP_INT_OPT(h->page_node, PGC_LABEL_PERIOD, h->page_period, h->global_period);  
      /* global no_border is used if page no_border is not found */
      PGC_XMLGETPROP_BOOL_OPT(h->page_node, PGC_LABEL_NO_BORDER, page->no_border, h->global_no_border, PGC_FALSE);	
      /* global widget_type is used if page widget_type not found */
      PGC_XMLGETPROP_WIDGETTYPE_OPT(h->page_node, PGC_LABEL_WIDGET, h->page_widget_type, h->global_widget_type, PGC_FALSE);	  /* global duration is used if page widget_type not found */
      PGC_XMLGETPROP_DOUBLE_OPT(h->page_node, PGC_LABEL_DURATION, h->page_duration, h->global_duration);	  

      /* Default value is 0 if rows not found */
      PGC_XMLGETPROP_INT_OPT(h->page_node, PGC_LABEL_ROWS, page->rows, 0);  
      h->var_node = h->page_node->children;
      h->page_node = h->page_node->next;
      h->last_error = PGC_STATUS_OK;
      return PGC_TRUE;
    }
  else
    {
      h->last_error = PGC_STATUS_NO_ITEM;
      return PGC_FALSE;
    }
 
}

int PGC_get_next_var(PGC_handle_t h, PGC_var_t* var)
{

  h->last_error = PGC_STATUS_FATAL;

  /* Iterate till next var */
  while( h->var_node && xmlIsBlankNode(h->var_node) )
    {
      h->var_node = h->var_node->next;
    }

  if(h->var_node)
    {
      PGC_XMLGETPROP_STRING(h->var_node, PGC_LABEL_NAME, var->name, PGC_FALSE);
      PGC_XMLGETPROP_VARTYPE(h->var_node, PGC_LABEL_TYPE, var->type, PGC_FALSE);
      PGC_XMLGETPROP_STRING_OPT(h->var_node, PGC_LABEL_LEGEND, var->legend, NULL);
      /* page period is used if variable period is not found */
      PGC_XMLGETPROP_INT_OPT(h->var_node, PGC_LABEL_PERIOD, var->period, h->page_period);  
      /* page widget_type is used if variable widget_type not found */
      PGC_XMLGETPROP_WIDGETTYPE_OPT(h->var_node, PGC_LABEL_WIDGET, var->widget_type, h->page_widget_type, PGC_FALSE);
      /* page period is used if variable period is not found */
      PGC_XMLGETPROP_DOUBLE_OPT(h->var_node, PGC_LABEL_DURATION, var->duration, h->page_duration);  
      /* For a title the widget is always a view */
      if(PGC_TITLE == var->type) 
	{
	  var->widget_type = PGC_WIDGET_VIEW;
	}
      h->var_node = h->var_node->next;
      h->last_error = PGC_STATUS_OK;
      return PGC_TRUE;
    }
  else 
    {
      h->last_error = PGC_STATUS_NO_ITEM;
      return PGC_FALSE;
    }  

 
}

PGC_error_t PGC_get_last_error(PGC_handle_t h)
{
  return h->last_error;
}


