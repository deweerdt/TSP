/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/datarwcpp.h,v 1.1 2003-01-22 22:56:58 sgalles Exp $

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

#ifndef _LIBUTIL_DATARWCPP_H
#define _LIBUTIL_DATARWCPP_H

#include <iostream>
#include <string>


namespace LibUtil
{

  class Datarwcpp
       {
	 enum direction_t
	   {
	     READER,
	     WRITER
	   };
	 
	 const std::string _file;
	 const direction_t _direction;	 

	 /* Avoid libUTIL C header in C++ header */
	 class hwrapper;
	 hwrapper* h;

       public:
	 
	 /* C'tor */
	 Datarwcpp(const std::string file, direction_t direction);
	 
	 /* D'tor */
	 virtual ~Datarwcpp() ;

	 

       };

};

#endif 
