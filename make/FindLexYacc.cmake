MESSAGE(STATUS "Looking for lex...")
FIND_PROGRAM(LEX_PROGRAM 
  NAMES lex flex 
  PATHS /usr/local/bin /usr/bin
  DOC "A lex-compatible lexer generator")
IF (LEX_PROGRAM) 
  MESSAGE(STATUS "Looking for lex... - found lex is ${LEX_PROGRAM}")
  SET(LEX_FOUND "YES")
ELSE (LEX_PROGRAM) 
  SET(LEX_FOUND "NO")
  MESSAGE(STATUS "Looking for lex... - NOT found")
ENDIF (LEX_PROGRAM) 

MESSAGE(STATUS "Looking for yacc...")
FIND_PROGRAM(YACC_PROGRAM 
  NAMES yacc bison
  PATHS /usr/local/bin /usr/bin
  DOC "A yacc-compatible parser generator")
IF (YACC_PROGRAM) 
  SET(YACC_FOUND "YES")
  MESSAGE(STATUS "Looking for yacc... - found yacc is ${YACC_PROGRAM}")
ELSE (YACC_PROGRAM)
  SET(YACC_FOUND "NO")
  MESSAGE(STATUS "Looking for yacc... - NOT found")
ENDIF (YACC_PROGRAM)