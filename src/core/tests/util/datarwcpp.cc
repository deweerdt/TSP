/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/datarwcpp.cc,v 1.1 2003-01-22 22:56:58 sgalles Exp $

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

/* Avoid libUTIL C header in C++ header */
class LibUtil::Datarwcpp::hwrapper
{
public :
  ::d_rhandle hr;
  ::d_rhandle hw;
};

using namespace std;


LibUtil::Datarwcpp::Datarwcpp(const std::string file, direction_t direction) : _file(file), _direction(direction), h(NULL)
{
  h = new hwrapper();
  assert(h);
}

LibUtil::Datarwcpp::~Datarwcpp()
{
}

