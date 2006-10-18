#ifndef _TEST_RPC_SVC_H_
#define _TEST_RPC_SVC_H_

#if defined(WIN32) && defined(TSP_SHARED_LIBS)
#  ifdef tsp_port_rpc_svc_EXPORTS
#    define _EXPORT_SERVER __declspec(dllexport) 
#  else
#    define _EXPORT_SERVER __declspec(dllimport)
#  endif
#else
#  undef _EXPORT_SERVER
#  define _EXPORT_SERVER
#endif

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

_EXPORT_SERVER void rpc_svc_main(int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif
