
IF (NOT DOXYGEN-NOTFOUND)
  
  TSP_CONFIGURE_FILE_AUTOTOOLS_COMPAT(
    ${CMAKE_CURRENT_SOURCE_DIR}/tsp.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/tsp 
    @ONLY)
  
  SET(DOXYGEN_INPUT tsp)
  SET(STATIC_PAGES  footer/footer.html header/header.html)
  SET(APIDOC_HTML_DIR ${DIST_DIR}/doc/api/html)
  SET(APIDOC_RTF_DIR ${DIST_DIR}/doc/api/rtf)
  SET(APIDOC_PDF_DIR ${DIST_DIR}/doc/api/pdf)
  
  ADD_CUSTOM_TARGET(apidoc
    COMMAND echo "Building C API documentation..."
    COMMAND mkdir -p ${APIDOC_HTML_DIR}
    COMMAND mkdir -p ${APIDOC_RTF_DIR}
    COMMAND mkdir -p ${APIDOC_PDF_DIR}
    COMMAND doxygen ${DOXYGEN_INPUT}
    COMMANE cp -f tsp.css ${APIDOC_HTML_DIR}/tsp.css
    COMMAND cp -r header ${APIDOC_HTML_DIR}
    COMMAND cp -r footer ${APIDOC_HTML_DIR}
    COMMAND cp -r images ${APIDOC_HTML_DIR}
    COMMAND echo "Building C API documentation...Done."
    )
ENDIF (NOT DOXYGEN-NOTFOUND)