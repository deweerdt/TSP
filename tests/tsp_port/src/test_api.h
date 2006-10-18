#ifndef _TEST_H_
#define _TEST_H_

#if defined (WIN32) && defined (TSP_SHARED_LIBS)
#  ifdef tsp_port_api_EXPORTS
#    define _EXPORT_API __declspec(dllexport) 
#  else
#    define _EXPORT_API __declspec(dllimport) 
#  endif
#else
#  undef _EXPORT_API
#  define _EXPORT_API
#include <stdarg.h>
#endif

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

typedef int (*affiche_appli_fp)(const char * format, ...);
typedef int (*saisie_appli_fp)(const char * format, ...);

_EXPORT_API void init_affichage(affiche_appli_fp function);
_EXPORT_API void init_saisie(saisie_appli_fp function);
_EXPORT_API int test_printf( char * format, ... );
_EXPORT_API int test_scan_1_car( char * uncar );
_EXPORT_API int api_main();
_EXPORT_API void printstruct(char * mes, void* val);
_EXPORT_API char* stringtosend();
_EXPORT_API int teststringrecu(char * recu);

#ifdef __cplusplus
}
#endif
#endif
