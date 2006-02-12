/*!  \file 

$Id: gdisp_xml.h,v 1.3 2006-02-12 00:13:56 erk Exp $

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

File      : Gdisp XML header file.

-----------------------------------------------------------------------
*/

#ifndef __XML_H__
#define __XML_H__


/*
 * XML includes.
 */
#include <libxml/encoding.h>
#ifdef XMLWRITER_SUPPORTED
  #include <libxml/xmlwriter.h>
#else
  typedef void* xmlTextWriterPtr; 
#endif
#include <libxml/tree.h>
#include <libxml/valid.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>


/*
 --------------------------------------------------------------------
                             DEFINITIONS
 --------------------------------------------------------------------
*/


#define GD_NO_COMPRESSION             0
#define GD_PREFERENCE_ENCODING "ISO-8859-1"

#define GD_INCREASE_INDENTATION       1
#define GD_DO_NOT_CHANGE_INDENTATION  0
#define GD_DECREASE_INDENTATION      -1


/*
 --------------------------------------------------------------------
                             PROTOTYPES
 --------------------------------------------------------------------
*/


xmlChar*    gdisp_xmlConvertInput     ( const char *in,
					const char *encoding );

void        gdisp_xmlErrorHandler     ( void       *voidValidContext,
					const char *format,
					... );

void        gdisp_xmlWarningHandler   ( void       *voidUserData,
					const char *format,
					... );

xmlNodeSet* gdisp_xmlGetChildren      ( xmlDoc  *document,
					xmlNode *startingNode,
					xmlChar *path );

xmlDoc*     gdisp_xmlParseAndValidate ( const char *filename );

void        gdisp_xmlGotoLine         ( xmlTextWriterPtr writer );

void        gdisp_xmlIndent           ( xmlTextWriterPtr  writer,
					xmlChar          *indentBuffer,
					char              mode );

gint        gdisp_xmlWriteAttributes  ( xmlTextWriterPtr  writer,
					gchar             indentationMode,
					xmlChar          *indentBuffer,
					xmlChar          *elementName,
					gboolean          endUpElement,
					... );

gint        gdisp_xmlWriteAttributeList ( xmlTextWriterPtr  writer,
					  gchar             indentationMode,
					  xmlChar          *indentBuffer,
					  xmlChar          *elementName,
					  gboolean          endUpElement,
					  GList            *attributeList );

void        gdisp_xmlGetAttributeList   ( xmlNode  *node,
					  GList   **attributeList );

guint       gdisp_getIntegerProperty    ( xmlChar *property,
					  guint    defaultValue );

#endif /* __XML_H__ */
