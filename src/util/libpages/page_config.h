#ifndef _PAGE_CONFIG_H_
#define _PAGE_CONFIG_H_

typedef struct PGC_instance_t PGC_instance_t;
typedef PGC_instance_t* PGC_handle_t;
     

#define PGC_TRUE 1
#define PGC_FALSE 0

struct PGC_global_t
{
  float display_frequency;
};

typedef struct PGC_global_t PGC_global_t;

struct PGC_page_t
{
  char* title;
  int x;
  int y;
  int width;
  int height;
  int rows;
  int is_visible;
  int no_border;
};

typedef struct PGC_page_t PGC_page_t;

enum PGC_var_type_t {PGC_DOUBLE = 0, PGC_TITLE, PGC_HEXA, PGC_BIN, PGC_STRING };
typedef enum PGC_var_type_t PGC_var_type_t;

enum PGC_widget_type_t {PGC_WIDGET_VIEW = 0, PGC_WIDGET_DRAW };
typedef enum PGC_widget_type_t PGC_widget_type_t;


enum PGC_error_t {PGC_STATUS_FATAL, PGC_STATUS_NO_ITEM, PGC_STATUS_OK };
typedef enum PGC_error_t PGC_error_t;


struct PGC_var_t
{
  char* name;
  char* legend;
  int period;
  float duration;  
  PGC_var_type_t type;
  PGC_widget_type_t widget_type; 
};  
typedef struct PGC_var_t PGC_var_t;

PGC_handle_t  PGC_open_file(char* filename);
int PGC_get_global(PGC_handle_t h, PGC_global_t* global);
int PGC_get_next_page(PGC_handle_t h, PGC_page_t* page);
int PGC_get_next_var(PGC_handle_t h, PGC_var_t* var);
int PGC_get_nb_page(PGC_handle_t h);
int PGC_get_nb_var(PGC_handle_t h);
int PGC_get_page_nb_var(PGC_handle_t h, int page);
PGC_error_t PGC_get_last_error(PGC_handle_t h);


/*PGC_bool_t PGC_save_config (PGC_handle_t h, char *filename);
PGC_bool_t PGC_load_config (PGC_handle_t h, char *filename);*/

#endif  /* _PAGES_CONFIG_H_ */
