
INSTALL(
  FILES 
  stub_ascii_writer_config.dat
  DESTINATION ${TSP_SHARE_BASEDIR}/example)

TSP_XINCLUDE(
  tsp_ascii_writer.h
  )
ADD_DEFINITIONS(-DYY_NO_UNPUT ${XML2_CFLAGS})
SET(ASCII_WRITER_SRC
  tsp_ascii_writer_main.c
  )
SET(ASCII_SRC
  tsp_ascii_writer.c
  )

SET(ASCII_YACC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tsp_ascii_writer_config.y)
SET(ASCII_YACC_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c)
SET(ASCII_LEX_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tsp_ascii_writer_config.lex)
SET(ASCII_LEX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tsp_ascii_writer_config_yy.c)

#Exec Lex
ADD_CUSTOM_COMMAND(
   OUTPUT ${ASCII_LEX_OUTPUT}
   COMMAND ${LEX_PROGRAM}
   ARGS -o${ASCII_LEX_OUTPUT} ${ASCII_LEX_SRC}
   DEPENDS ${ASCII_LEX_SRC}
   )
SET(ASCII_SRC ${ASCII_SRC}
${ASCII_LEX_OUTPUT})

#Exec Yacc
ADD_CUSTOM_COMMAND(
   OUTPUT ${ASCII_YACC_OUTPUT}
   COMMAND ${YACC_PROGRAM}
   ARGS ${YACC_COMPAT_ARG} -d ${ASCII_YACC_SRC}
   DEPENDS ${ASCII_YACC_SRC}
   )
SET(ASCII_SRC ${ASCII_SRC}
${ASCII_YACC_OUTPUT})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/consumers/ascii_writer/)

ADD_EXECUTABLE(tsp_ascii_writer-bin ${ASCII_WRITER_SRC})
SET_TARGET_PROPERTIES(tsp_ascii_writer-bin
  PROPERTIES OUTPUT_NAME tsp_ascii_writer)
ADD_LIBRARY(tsp_ascii_writer ${ASCII_SRC})
ADD_DEPENDENCIES(tsp_ascii_writer-bin tsp_ascii_writer)

	
TARGET_LINK_LIBRARIES(tsp_ascii_writer-bin tsp_ascii_writer tsp_consumer tsp_common tsp_services
  ${PTHREAD_LIBRARY_NAME} tspcfg ${XML2_LIBRARY})

TSP_LIBRARY_INSTALL(tsp_ascii_writer)
TSP_PROGRAM_INSTALL(tsp_ascii_writer-bin)
