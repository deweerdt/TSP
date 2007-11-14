/*

$Id: gdisp_xml.c,v 1.11 2007-11-14 21:53:20 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi.

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
Maintainer: tsp@astrium-space.com
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Graphic Tool XML management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"
#include "gdisp_xml.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * gdisp_onvertInput :
 *   @in       : string in a given encoding
 *   @encoding : the encoding used
 *
 * Converts @in into UTF-8 for processing with libxml2 APIs.
 *
 * Returns the converted UTF-8 string, or NULL in case of error.
 *
 * CAUTION : original source code of this procedure may be found
 * at http://xmlsoft.org/examples/testWriter.c.
 *
 */
xmlChar*
gdisp_xmlConvertInput ( const char *in,
			const char *encoding )
{
  xmlChar                   *out     = (xmlChar*)NULL;
  xmlCharEncodingHandlerPtr  handler = (xmlCharEncodingHandlerPtr)NULL;
  int                        ret     = 0;
  int                        size    = 0;
  int                        outSize = 0;
  int                        temp    = 0;

  /*
   * Check.
   */
  if (in == (const char*)NULL) {
    return (xmlChar*)NULL;
  }

  handler = xmlFindCharEncodingHandler(encoding);

  if (handler == (xmlCharEncodingHandlerPtr)NULL) {
    return (xmlChar*)NULL;
  }

  size    = (int)strlen(in) + 1;
  outSize = size * 2 - 1;
  out     = (unsigned char*)xmlMalloc((size_t)outSize);

  if (out != (unsigned char*)NULL) {

    temp = size - 1;
    ret  = handler->input(out,
			  &outSize,
			  (const xmlChar*)in,
			  &temp);

    if ((ret < 0) || (temp - size + 1)) {

      xmlFree(out);
      out = (unsigned char*)NULL;

    }
    else {

      out = (unsigned char*)xmlRealloc(out,outSize + 1);
      out[outSize] = '\0';  /* null terminating out */

    }

  }

  return out;

}


/*
 * XML error handler.
 */
void
gdisp_xmlErrorHandler (void       *voidValidContext,
		       const char *format,
		       ... )
{

  va_list variableList;
  char    tmpBuffer[1024];

  va_start(variableList,format);
  vsprintf(tmpBuffer,format,variableList);

  fprintf(stdout,
	  "TARGA : ERROR <%s> : %s\n",
	  (unsigned char*)voidValidContext,
	  tmpBuffer);

  fflush (stdout);
	
  va_end(variableList);

}


/*
 * XML warning handler.
 */
void
gdisp_xmlWarningHandler (void       *voidUserData,
			 const char *format,
			 ... )
{

  va_list variableList;
  char    tmpBuffer[1024];
	
  va_start(variableList,format);
  vsprintf(tmpBuffer,format,variableList);

  fprintf(stdout,
	  "TARGA : WARNING <%s> : %s\n",
	  (unsigned char*)voidUserData,
	  tmpBuffer);

  fflush (stdout);
	
  va_end(variableList);

}


/*
 * XML get children from a given path expression.
 */
xmlNodeSet*
gdisp_xmlGetChildren ( xmlDoc  *document,
		       xmlNode *startingNode,
		       xmlChar *path )
{

  xmlXPathContext *context = (xmlXPathContext*)NULL;
  xmlXPathObject  *result  =  (xmlXPathObject*)NULL;
  xmlNodeSet      *nodeSet =      (xmlNodeSet*)NULL;

  /*
   * Check.
   */
  if (document == (xmlDoc*)NULL || path == (xmlChar*)NULL) {
    return (xmlNodeSet*)NULL;
  }

  /*
   * Create a temporary context.
   */
  context = xmlXPathNewContext(document);
  if (context == (xmlXPathContext*)NULL) {
    return (xmlNodeSet*)NULL;
  }

  context->node = startingNode; /* may be null */

  /*
   * Evaluate expression.
   */
  result = xmlXPathEvalExpression(path,context);

  /*
   * Free temporary context.
   */
  nodeSet = result->nodesetval;
  result->nodesetval = (xmlNodeSet*)NULL;

  xmlXPathFreeObject ( result);
  xmlXPathFreeContext(context);

  /*
   * Check empty node.
   */
  if (xmlXPathNodeSetIsEmpty(nodeSet)) {
    xmlXPathFreeNodeSet(nodeSet);
    return (xmlNodeSet*)NULL;
  }

  return nodeSet;

}


/*
 * XML parse and validate a wholme document.
 */
xmlDoc*
gdisp_xmlParseAndValidate ( const char *filename )
{

  xmlValidCtxt *validationContext = (xmlValidCtxt*)NULL;
  xmlDoc       *document          =       (xmlDoc*)NULL;
  int           documentIsValid   =                   0;
	
  /*
   * Allocate memory for DTD validation.
   */
  validationContext = (xmlValidCtxt*)g_malloc0(sizeof(xmlValidCtxt));

  if (validationContext == (xmlValidCtxt*)NULL) {
    return (xmlDoc*)NULL;
  }
	
  memset(validationContext,0,sizeof(xmlValidCtxt));

  validationContext->userData = (void*)filename;
  validationContext->error    = gdisp_xmlErrorHandler;
  validationContext->warning  = gdisp_xmlWarningHandler;

  /*
   * Parse XML document.
   */
  document = xmlParseFile(filename);

  if (document == (xmlDoc*)NULL) {
    return (xmlDoc*)NULL;
  }

  /*
   * DTD Validation.
   */
#if defined (GD_USE_DTD)
  documentIsValid = xmlValidateDocument(validationContext,
					document);
#else
  documentIsValid = 1;
#endif

  /*
   * Free memory.
   */
  g_free(validationContext);

  /*
   * Return to calling process.
   */
  if (documentIsValid == 0) {

    xmlFreeDoc(document);
    document = (xmlDoc*)NULL;

  }

  return document;

}


/*
 * Goto line in the XML file by inserting a carriage return.
 */
void
gdisp_xmlGotoLine ( xmlTextWriterPtr writer )
{
#ifdef XMLWRITER_SUPPORTED
  /*
   * Goto line.
   */
  xmlTextWriterWriteRaw(writer,
			(xmlChar*)"\n");
#endif

}


/*
 * Indentation management.
 * Mode :  1 - increase identation
 *         0 - do not change indentation
 *        -1 - decrease identation
 */
void
gdisp_xmlIndent( xmlTextWriterPtr  writer,
		 xmlChar          *indentBuffer,
		 char              mode )
{

  xmlChar spaces[3] = { ' ', ' ', '\0' };
  gint    nbSpaces  = xmlStrlen(spaces);

  if (mode == GD_INCREASE_INDENTATION) {
    strcat(indentBuffer,spaces);
  }
  else if (mode == GD_DECREASE_INDENTATION) {
    if (xmlStrlen(indentBuffer) >= nbSpaces) {
      indentBuffer[xmlStrlen(indentBuffer) - nbSpaces] = '\0';
    }
  }
#ifdef XMLWRITER_SUPPORTED
  xmlTextWriterWriteRaw(writer,
			(xmlChar*)indentBuffer);
#endif

}


/*
 * Write an element and its attributes.
 */
gint
gdisp_xmlWriteAttributes( xmlTextWriterPtr  writer,
			  gchar             indentationMode,
			  xmlChar          *indentBuffer,
			  xmlChar          *elementName,
			  gboolean          endUpElement,
			  ... )
{

  gint     errorCode       = 0;

#ifdef XMLWRITER_SUPPORTED 

  xmlChar *attributeName  = (xmlChar*)NULL;
  xmlChar *attributeValue = (xmlChar*)NULL;
  va_list  argList;

  /*
   * Goto line and ident.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    indentationMode);
  errorCode = xmlTextWriterStartElement(writer,
					elementName);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return errorCode;
  }


  /*
   * Start after the first argument.
   */
  va_start(argList,endUpElement);


  /*
   * Loop until a NULL argument is encountered.
   */
  do {


    /*
     * Retreive attribute name and value.
     */
    attributeName = va_arg(argList,xmlChar*);
    if (attributeName != (xmlChar*)NULL) {

      attributeValue = va_arg(argList,xmlChar*);

      errorCode = xmlTextWriterWriteAttribute(writer,
					      attributeName,
					      attributeValue);

      if (errorCode < 0) {
	/* No need to print any error message, because gdisp+ is exiting */
	return errorCode;
      }

    }

  } while (attributeName != (xmlChar*)NULL);


  /*
   * End up element if requested.
   */
  if (endUpElement == TRUE) {

    errorCode = xmlTextWriterEndElement(writer);

  }

#else 

  errorCode = 1;

#endif

  return errorCode;

}


/*
 * Write an element and its attributes.
 */
gint
gdisp_xmlWriteAttributeList ( xmlTextWriterPtr  writer,
			      gchar             indentationMode,
			      xmlChar          *indentBuffer,
			      xmlChar          *elementName,
			      gboolean          endUpElement,
			      GList            *attributeList )
{

  gint     errorCode       = 0;
  xmlChar *attributeName  = (xmlChar*)NULL;
  xmlChar *attributeValue = (xmlChar*)NULL;

  /*
   * Goto line and ident.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    indentationMode);

#ifdef XMLWRITER_SUPPORTED
  errorCode = xmlTextWriterStartElement(writer,
					elementName);
#endif

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return errorCode;
  }


  /*
   * Loop until the end of the list.
   * It is assumed that the list is a list of pairs.
   */
  attributeList = g_list_first(attributeList);
  while (attributeList != (GList*)NULL) {

    /*
     * Retreive attribute name and value.
     */
    attributeName  = attributeList->data;

    attributeList  = g_list_next(attributeList);
    attributeValue = attributeList->data;

#ifdef XMLWRITER_SUPPORTED
    errorCode = xmlTextWriterWriteAttribute(writer,
					    attributeName,
					    attributeValue);
#endif

    if (errorCode < 0) {
      /* No need to print any error message, because gdisp+ is exiting */
      return errorCode;
    }

    attributeList = g_list_next(attributeList);

  }


  /*
   * End up element if requested.
   */
  if (endUpElement == TRUE) {
#ifdef XMLWRITER_SUPPORTED
    errorCode = xmlTextWriterEndElement(writer);
#endif

  }

  return errorCode;

}


/*
 * Get back the attribute list of a node.
 * Return this list into a GList object.
 */
void
gdisp_xmlGetAttributeList ( xmlNode  *node,
			    GList   **attributeList )
{

  xmlAttr *nodeProperties = (xmlAttr*)NULL;

  /*
   * Loop over all properties.
   */
  nodeProperties = node->properties;

  while (nodeProperties != (xmlAttr*)NULL) {

    *attributeList = g_list_append(*attributeList,
				   (gpointer)nodeProperties->name);

    *attributeList = g_list_append(*attributeList,
				   (gpointer)nodeProperties->children->content);

    nodeProperties = nodeProperties->next;

  }

}


/*
 * Get integer property.
 */
guint
gdisp_getIntegerProperty ( xmlChar *property,
			   guint    defaultValue )
{

  /*
   * Get back default value if input property is null.
   */
  if (property == (xmlChar*)NULL) {
    return defaultValue;
  }
  else {
     return atoi((char*)property);
  }

}

