/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/Attic/datarwcpp.cc,v 1.3 2003-02-06 22:43:18 sgalles Exp $

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

extern "C"
{
#define _LIBUTIL_REENTRANT
#include "libUTIL.h"
}

using namespace std;


/* Avoid libUTIL C header in C++ header */
class LibUtil::Datarwcpp::hwrapper
{
public :
  ::d_rhandle hr;
  ::d_rhandle hw;

};


LibUtil::Datarwcpp::Datarwcpp() : 
_vec_fbuf(NULL),
_vec_dbuf(NULL),
_fbuf(NULL),
_dbuf(NULL),
_h(NULL),
_nb_records(-1)
{
  _h = new hwrapper();
  assert(_h);
}

LibUtil::Datarwcpp::~Datarwcpp()
{
  delete(_h); _h = NULL;
  if(_dbuf) delete[](_dbuf);
  if(_fbuf) delete[](_fbuf);
  if(_vec_dbuf) delete(_vec_dbuf);
  if(_vec_fbuf) delete(_vec_fbuf);
}

bool LibUtil::Datarwcpp::r_open(const std::string& file)
{

  int iuse_dbl;
  _h->hr = d_ropen_r((char*)file.c_str(), &iuse_dbl);

  if(_h->hr)
    {

      _use_double = ((iuse_dbl != 0) ? true : false);

      _nb_records = r_get_intern_nb_rec();  
      int nb_vars  = r_get_intern_nb_vars();
      int nb_comments  = r_get_intern_nb_comments();

      /* init buf */
      if(_use_double)
	{
	  /* Do ask me about the '+1' I believe that lib_res needs it,
	     but I can't remeber why...whatever...*/
	  _dbuf = new double[nb_vars+1];
	  _vec_dbuf = new std::vector<double>(nb_vars);
	}
      else
	{
	  _fbuf = new float[nb_vars+1];
	  _vec_fbuf = new std::vector<float>(nb_vars);
	}

      {
	char namev[RES_NAME_LEN];
	char descv[RES_DESC_LEN];
	for(int i=0 ; i< nb_vars ; i++)
	  {
	    d_rnam_r(_h->hr, namev, descv, i);
	    _vars_info.push_back( VarInfo(namev,descv) );
	
	  }
      }

      {
	char coms[RES_COM_LEN];    
	for(int i=0 ; i< nb_comments ; i++)
	  {
	    d_rcom_r(_h->hr, coms, i);
	    _comments.push_back(coms);
	
	  }
      }
  
    }

  return ( (_h->hr != 0) ? true : false);

}


int  LibUtil::Datarwcpp::r_get_intern_nb_rec() const
{ 
   assert(_h->hr);
   return d_rval_r(_h->hr, 'r');
}

int  LibUtil::Datarwcpp::r_get_intern_nb_vars() const 
{ 
   assert(_h->hr);
   return d_rval_r(_h->hr, 'v');
}


int  LibUtil::Datarwcpp::r_get_intern_nb_comments() const 
{ 
   assert(_h->hr);
   return d_rval_r(_h->hr, 'c');
}


const std::vector<LibUtil::VarInfo>& LibUtil::Datarwcpp::r_get_vars_info() const
{
  assert(_h->hr);
  return _vars_info;
}

const std::vector<std::string>& LibUtil::Datarwcpp::r_get_comments() const
{
  assert(_h->hr);
  return _comments;
}

bool LibUtil::Datarwcpp::r_is_double() const
{
  assert(_h->hr);
  return _use_double;
}

bool  LibUtil::Datarwcpp::r_get_nb_rec() const
{
  assert(_h->hr);
  return _nb_records;
}

bool LibUtil::Datarwcpp::r_update_buf()
{
   assert(_h->hr);

   bool ret = false;
   if(_use_double)
     {
       int n = d_read_r (_h->hr, _dbuf);
       if(n != EOF)
	 {
	   ret = true;
	   for (int i = 0 ; i <  _vec_dbuf->size() ;  i++)
	     {
	       (*_vec_dbuf)[i] = _dbuf[i];
	     }
	 }
       
     }
   else
     {
       int n = d_read_r (_h->hr, _fbuf);
       if(n != EOF)
	 {
	   ret = true;
	   for (int i = 0 ; i <  _vec_fbuf->size() ;  i++)
	     {
	       (*_vec_fbuf)[i] = _fbuf[i];	       
	     }
	 }
       
     }

   return ret;
}


const std::vector<float>&  LibUtil::Datarwcpp::r_get_float_buf() const
{

  assert(!r_is_double());
  return (*_vec_fbuf);
}

const std::vector<double>&  LibUtil::Datarwcpp::r_get_double_buf() const
{
  assert(r_is_double());
  return (*_vec_dbuf);
}
