/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libutil/Attic/hash.h,v 1.1 2005-03-30 09:34:27 tractobob Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyleft (c) 2005 Robert PAGNOT 


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium.eads.net
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Hash Table implementation

-----------------------------------------------------------------------
 */


#ifndef _HASH_H
#define _HASH_H

struct hash_table_t {
  void                 *id;
  int                  next[127];
  /* 127 items are not actually used, since next[O] is for (first) character
     and next[last - first] is for (last) character as defined in hash_open
     function call.
     Even if last-first > 127, overflow will behave correctly because of
     MemoryPool use strategy : HASH_MEMPOOL_LENGTH
  */
};

typedef struct hash_table_t hash_table_t;


typedef struct {
  char *memPool;

  int nb_tables;
  int nb_strings;
  int nb_chars;

  int first;
  int last;

  int table;

} hash_t;

hash_t* hash_open(int first, int last);
int hash_append(hash_t *hash, char *string, void *id);
void *hash_get(hash_t *hash, char *string);
int hash_close(hash_t *hash);
void hash_dump(hash_t *hash, int level);


#endif /* _HASH_H */
