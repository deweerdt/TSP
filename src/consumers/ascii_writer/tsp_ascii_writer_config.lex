/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer_config.lex,v 1.2 2004-09-22 20:18:56 erk Exp $

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
  extern char* yytext;
int yyerror(char *msg) {
  printf("tsp_ascii_writer_config: line %d, col %d : %s at '%s'\n", 
	 tsp_ascii_writer_lineno, tsp_ascii_writer_colno, msg, yytext);
  ++tsp_ascii_writer_parse_error;
  return 0;
}
%}
T_SAMPLE_SYMBOL   [[:alpha:]][[:alnum:]_\[\]]*
T_PERIOD          [[:digit:]]*
T_SPACE           [[:blank:]]*
T_NEWLINE         \t*\n
T_COMMENT         [#].*
%% 
{T_SAMPLE_SYMBOL}  {tsp_ascii_writer_add_var(yytext); tsp_ascii_writer_colno+=yyleng; return T_SAMPLE_SYMBOL;}
{T_PERIOD}         {tsp_ascii_writer_add_var_period(atoi(yytext)); tsp_ascii_writer_colno+=yyleng; return T_PERIOD;}
{T_NEWLINE}        {++tsp_ascii_writer_lineno;tsp_ascii_writer_colno = 0;  return T_NEWLINE;}
{T_COMMENT}        {tsp_ascii_writer_add_comment(yytext); tsp_ascii_writer_colno+=yyleng; return T_COMMENT;}
{T_SPACE}          {tsp_ascii_writer_colno+=yyleng; return T_SPACE;}
%%
 
