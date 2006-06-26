ADD_DEFINITIONS(-DYY_NO_UNPUT ${XML2_FLAGS})
SET(ASCII_SRC
tsp_ascii_writer.c
tsp_ascii_writer_main.c
)

SET(ASCII_YACC_SRC tsp_ascii_writer_config.y)
SET(ASCII_YACC_OUTPUT y.tab.c)
SET(ASCII_LEX_SRC tsp_ascii_writer_config.lex)
SET(ASCII_LEX_OUTPUT tsp_ascii_writer_config_yy.c)

#Exec Lex
ADD_CUSTOM_COMMAND(
   OUTPUT ${ASCII_LEX_OUTPUT}
   COMMAND lex
   ARGS -o${ASCII_LEX_OUTPUT} ${ASCII_LEX_SRC}
   DEPENDS ${ASCII_LEX_SRC}
   )
SET(ASCII_SRC ${ASCII_SRC}
${ASCII_LEX_OUTPUT})

#Exec Yacc
ADD_CUSTOM_COMMAND(
   OUTPUT ${ASCII_YACC_OUTPUT}
   COMMAND yacc
   ARGS -d ${ASCII_YACC_SRC}
   DEPENDS ${ASCII_YACC_SRC}
   )
SET(ASCII_SRC ${ASCII_SRC}
${ASCII_YACC_OUTPUT})

INCLUDE_DIRECTORIES(.)

ADD_EXECUTABLE(tsp_ascii_writer ${ASCII_SRC})
ADD_LIBRARY(tsp_ascii_writer ${ASCII_SRC})
	
TARGET_LINK_LIBRARIES(tsp_ascii_writer tsp_consumer tsp_common tsp_services
pthread tspcfg ${XML2_LIBS})

