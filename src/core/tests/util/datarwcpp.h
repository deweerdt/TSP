/*!  \file

$Header: /home/def/zae/tsp/tsp/src/core/tests/util/Attic/datarwcpp.h,v 1.3 2003-01-29 23:12:39 sgalles Exp $

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

namespace LibUtil
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

    };

  class ResInfo
    {
      
      std::vector<VarInfo> _VarsInfo;
      std::vector<std::string> _comments;
      int _nb_rec;
      bool _use_double;

    public:
      void add_comment(const std::string& comment) { _comments.push_back(comment); }
      void add_var_info(const VarInfo& var) { _VarsInfo.push_back(var); }
      void set_use_double(bool use_double) { _use_double = use_double; }
      void set_nb_rec(int nb_rec) { _nb_rec = nb_rec; }
    };

  class Datarwcpp
    {
	 
    public:
      enum usage_t
	{
	  READER,
	  WRITER
	};
      
    private:
	 /** wrapper class for C libUTIL handles.
	  * Avoid C header in C++ header
	  */
	 class hwrapper;
 
	 
	 const std::string _file;
	 const usage_t _usage;	 
	 hwrapper* _h;

	 std::vector<VarInfo> _vars_info;
	 std::vector<std::string> _comments;
	 bool _use_double;
	 int _nb_records;
	 int _nb_vars;
	 int _nb_comments;

       public:
	 
	 /* C'tor */
	 Datarwcpp(const std::string& file, usage_t usage);
	 
	 /* D'tor */
	 virtual ~Datarwcpp() ;

	 /* Read */
	 bool ropen();
	 int rget_nb_records() const ;
	 int rget_nb_vars() const ;
	 int rget_nb_comments() const ;
	 const std::vector<VarInfo>& rget_vars_info() const;
	 const std::vector<std::string>& rget_comments() const;
	 
       private:
	 int rget_intern_nb_records() const ;
	 int rget_intern_nb_vars() const ;
	 int rget_intern_nb_comments() const ;


       };

};

#endif 
