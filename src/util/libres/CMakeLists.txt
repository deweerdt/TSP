SET(LIBRES_SRC
use_dbl.c datar.c
dataw.c)

ADD_LIBRARY(tsp_res ${LIBRES_SRC})
TSP_LIBRARY_INSTALL(tsp_res)
