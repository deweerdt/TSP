/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/datarwcpp.cc,v 1.2 2003-01-28 22:54:12 sgalles Exp $

-----------------------------------------------------------------------

libUTIL - core components to read and write res files

Copyright (c) 2002 Marc LE ROY and TSP developpement team

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
Maintainer : tsp@astrium-space.com
Component : libUTIL, res read and writer, cpp wrapper

-----------------------------------------------------------------------

Purpose   :

-----------------------------------------------------------------------
 */

#include <assert.h>

#include "datarwcpp.h"

#define _LIBUTIL_REENTRANT
#include "libUTIL.h"

using namespace std;

#define CHECK_USAGE(target, ret)   \
{ \
     assert( _usage == target ); \
     if ( _usage != target ) return ret; \
} while(0)


/* Avoid libUTIL C header in C++ header */
class LibUtil::Datarwcpp::hwrapper
{
public :
  ::d_rhandle hr;
  ::d_rhandle hw;

};


LibUtil::Datarwcpp::Datarwcpp(const string& file, usage_t usage) : 
_file(file), 
_usage(usage), 
_h(NULL)
{
  _h = new hwrapper();
  assert(_h);
}

LibUtil::Datarwcpp::~Datarwcpp()
{
  delete(_h); _h = NULL;
}

bool LibUtil::Datarwcpp::ropen(LibUtil::ResInfo& info)
{

  CHECK_USAGE(READER, false);  
  
  int iuse_dbl;
  _h->hr = d_ropen_r((char*)_file.c_str(), &iuse_dbl);
  info.set_use_double((iuse_dbl != 0) ? true : false);

  return ( (_h->hr != 0) ? true : false);

}

int  LibUtil::Datarwcpp::rget_nb_rec() const
{
   CHECK_USAGE(READER, 0);  
   assert(_h->hr);

   return d_rval_r(_h->hr, 'r');
}

int  LibUtil::Datarwcpp::rget_nb_var() const 
{
   CHECK_USAGE(READER, 0);  
   assert(_h->hr);

   return d_rval_r(_h->hr, 'v');
}

int  LibUtil::Datarwcpp::rget_nb_com() const 
{
   CHECK_USAGE(READER, 0);  
   assert(_h->hr);

   return d_rval_r(_h->hr, 'c');
}
