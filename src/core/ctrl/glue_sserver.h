/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/Attic/glue_sserver.h,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the glue server

-----------------------------------------------------------------------
 */

#ifndef _TSP_GLUESERVER_H
#define _TSP_GLUESERVER_H

#include "tsp_ringbuf.h"

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"



struct glu_item_t
{
    time_stamp_t time;
    int provider_global_index;
    double value;

};

typedef struct glu_item_t glu_item_t;

RINGBUF_DECLARE_TYPE(glu_ringbuf,glu_item_t, RINGBUF_SZ(GLU_RING_BUFSIZE));

void  GLU_get_sample_symbol_info_list
	(TSP_sample_symbol_info_list_t* symbol_list);

int GLU_add_block(int provider_global_index, xdr_and_sync_type_t type);

int GLU_commit_add_block(void);

char* GLU_get_server_name(void);

int TSP_glue_sserver_init(void);

glu_ringbuf* GLU_get_ringbuf(void);

#endif /*_TSP_GLUESERVER_H*/
