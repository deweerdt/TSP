/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer_config.lex,v 1.1 2004-09-21 21:59:58 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : TSP ascii writer config file analyzer

-----------------------------------------------------------------------
 */
%{
  /* for atoi() see below */
#include <math.h>
#include <stdio.h>
#include <tsp_ascii_writer.h>
#include "tsp_ascii_writer_config.tab.h"
int yyerror(char *msg) {
  printf("%d : %s at '%s'\n", 
	 tsp_ascii_writer_nb_line, msg, yytext);
  return 0;
}
%}
T_SAMPLE_SYMBOL   [[:alpha:]][[:alnum:]_]*
T_PERIOD          [[:digit:]]*
T_SPACE           [[:blank:]]*
T_NEWLINE         \t*\n
T_COMMENT         [#].*
%% 
{T_SAMPLE_SYMBOL}  {tsp_ascii_writer_add_var(yytext); return T_SAMPLE_SYMBOL;}
{T_PERIOD}         {tsp_ascii_writer_add_var_period(atoi(yytext)); return T_PERIOD;}
{T_NEWLINE}        {++tsp_ascii_writer_nb_line; return T_NEWLINE;}
{T_COMMENT}        {tsp_ascii_writer_add_comment(yytext); return T_COMMENT;}
{T_SPACE}          {return T_SPACE;}
%%
 
