#ifndef _TEST_RPC_CLNT_H_
#define _TEST_RPC_CLNT_H_

#if defined (WIN32) && defined (TSP_SHARED_LIBS)
#  ifdef tsp_port_rpc_clnt_EXPORTS
#     define _EXPORT_CLIENT __declspec(dllexport) 
#  else
#     define _EXPORT_CLIENT __declspec(dllimport) 
#  endif
#else
#   undef _EXPORT_CLIENT
#  define _EXPORT_CLIENT
#endif

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

_EXPORT_CLIENT void rpc_clnt_main(int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif
