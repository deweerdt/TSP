
%{
#define YYDEBUG 1
#define YYERROR_VERBOSE
#define YYSTYPE double
#include <stdio.h>
  YYSTYPE yylval; 
  int yyerror(char *msg);
  int yylex();
#include <tsp_ascii_writer.h>
%}
%token T_SAMPLE_SYMBOL T_PERIOD T_SPACE T_NEWLINE T_COMMENT
%start INPUT
%%
INPUT : LINE 
      | LINE INPUT
;
LINE : SAMPLE_LINE T_NEWLINE
     | COMMENT_LINE T_NEWLINE
     | VOID_LINE T_NEWLINE
     | VOID_LINE
     | T_NEWLINE
     | error T_NEWLINE 
;
SAMPLE_LINE : T_SAMPLE_SYMBOL T_SPACE T_PERIOD 
            | T_SAMPLE_SYMBOL T_SPACE T_PERIOD T_SPACE T_COMMENT
;
COMMENT_LINE : T_COMMENT
;
VOID_LINE : T_SPACE
; 
%% 
#ifdef YACCMAIN
int main( void ) {
  /*  yydebug = 1; */
  yyparse();
  return 0;
}
#endif

