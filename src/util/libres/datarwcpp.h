/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/datarwcpp.h,v 1.4 2003-02-24 23:10:30 sgalles Exp $

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
#include <vector>

namespace LibRes
{

  class VarInfo
    {
      std::string _name;
      std::string _description;

    public:
      VarInfo() {}
      VarInfo (const std::string& name, const std::string& description) :
	_name(name), _description(description) {}
	
      ~VarInfo() {}
      const VarInfo& VarInfo::operator= (const VarInfo& right)
	{
	  _name = right.get_name();
	  _description = right.get_description();
	}

      const std::string& get_name() const { return _name; }
      const std::string& get_description() const { return _description; }   

      void print() const {  std::cout << "var='"<<  get_name() << 
		      "' desc='" << get_description() << "'" << std::endl; }

    };

  
 class ResReader
    {
	 
    private:
	 /** wrapper class for C libUTIL handles.
	  * Avoid C header in C++ header
	  */
	 class hwrapper;
	 hwrapper* _h;

	 std::vector<VarInfo> _vars_info;
	 std::vector<std::string> _comments;
	 std::vector<float>* _vec_fbuf;
	 std::vector<double>* _vec_dbuf;
	 float* _fbuf;
	 double* _dbuf;
	 

	 bool _use_double;
	 int _nb_records;

       public:
	 
	 /* C'tor */
	 ResReader();
	 
	 /* D'tor */
	 virtual ~ResReader() ;
	 
	 bool open(const std::string& file);

	 const std::vector<VarInfo>& get_vars_info() const;
	 const std::vector<std::string>& get_comments() const;

	 bool is_double() const;
	 bool get_nb_rec() const;

	 void get_next_rec(const std::vector<float>*& rec);
	 void get_next_rec(const std::vector<double>*& rec);
	 
       private:
	 bool update_buf();
	 int get_intern_nb_rec() const ;
	 int get_intern_nb_vars() const ;
	 int get_intern_nb_comments() const ;

       };

 class ResWriter
    {
	 
    private:
	 /** wrapper class for C libUTIL handles.
	  * Avoid C header in C++ header
	  */
	 class hwrapper;
	 hwrapper* _h;

	 bool _use_double;
	 int _nb_vars;

       public:
	 
	 /* C'tor */
	 ResWriter();
	 
	 /* D'tor */
	 virtual ~ResWriter() ;

	 /* Read */
	 bool open(const std::string& file,
		   const std::vector<VarInfo>& vars,
		   const std::vector<std::string>& coms);
	 
	 bool add_rec(const std::vector<float>& rec);
	 bool add_rec(const std::vector<double>& rec);

       };


};

#endif 
