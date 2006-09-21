/*

$Id: gdisp_preferences.c,v 1.8 2006-09-21 20:19:59 esteban Exp $

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

File      : Graphic Tool Preference management.

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
 * Load preferences.
 */
static void
gdisp_loadPreferences ( Kernel_T *kernel,
			xmlDoc   *document )
{

  xmlNodeSet   *preferenceTableNode = (xmlNodeSet*)NULL;
  xmlNode      *preferenceNode      = (xmlNode*)NULL;
  xmlChar      *propertyValue       = (xmlChar*)NULL;
  unsigned int  cptPreference       = 0;


  /*
   * Get back information.
   */
  preferenceTableNode =
    gdisp_xmlGetChildren(document,(xmlNode*)NULL,"//Preferences/preference");

  if (preferenceTableNode != (xmlNodeSet*)NULL &&
      preferenceTableNode->nodeNr > 0) {

    for (cptPreference=0;
	 cptPreference<preferenceTableNode->nodeNr;
	 cptPreference++) {

      preferenceNode = preferenceTableNode->nodeTab[cptPreference];

      propertyValue = xmlGetProp(preferenceNode,
				 preferenceNode->properties->name);

      gdisp_setUpPreferenceFromString(kernel,
				      (gchar*)preferenceNode->properties->name,
				      (gchar*)propertyValue);

      xmlFree(propertyValue);

    }

  }


  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(preferenceTableNode);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Preference Management : load preferences from '.gdisp+' XML file
 * located in the HOME directory of the user.
 */
void
gdisp_loadPreferenceFile ( Kernel_T *kernel )
{

  FILE    *stream   = (FILE*)NULL;
  xmlDoc  *document = (xmlDoc*)NULL;
  xmlChar  completeFilename[256];


  /*
   * Caution.
   */
  if (g_get_home_dir() == (char*)NULL) {
    /* strange, but never mind, keep on with default preferences */
    return;
  }
  sprintf(completeFilename,
	  "%s%c.targa",
	  g_get_home_dir(),
	  G_DIR_SEPARATOR);

  /*
   * Check file disponibility.
   */
  stream = fopen(completeFilename,"r");
  if (stream == (FILE*)NULL) {
    return;
  }
  else {
    fclose(stream);
  }

  /*
   * This initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION


  /*
   * Parse and validate document.
   */
  document = gdisp_xmlParseAndValidate(completeFilename);
  if (document == (xmlDoc*)NULL) {
    return;
  }


  /*
   * Load preferences.
   */
  gdisp_loadPreferences(kernel,
			document);


  /*
   * Free document.
   */
  xmlFreeDoc(document);

}


/*
 * Preference Management : save preferences into '.gdisp+' XML file
 * located in the HOME directory of the user.
 */
void
gdisp_savePreferenceFile ( Kernel_T *kernel )
{

#if defined(XMLWRITER_SUPPORTED)

  int               errorCode      = 0;
  xmlTextWriterPtr  writer         = (xmlTextWriterPtr)NULL;
  xmlChar          *attributeValue = (xmlChar*)NULL;
  xmlChar          *tmp            = (xmlChar*)NULL;
  xmlChar           indentBuffer    [256];
  xmlChar           completeFilename[256];
  xmlChar           stringValue     [256];

  /*
   * Caution.
   */
  indentBuffer[0] = '\0';
  if (g_get_home_dir() == (char*)NULL) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }
  sprintf(completeFilename,
	  "%s%c.targa",
	  g_get_home_dir(),
	  G_DIR_SEPARATOR);

  /*
   * This initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*
   * Create a new XmlWriter for '.targa', with no compression.
   */
  writer = xmlNewTextWriterFilename(completeFilename,
				    GD_NO_COMPRESSION);

  if (writer == (xmlTextWriterPtr)NULL) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Start the document with the xml default for the version,
   * encoding ISO 8859-1 and the default for the standalone
   * declaration.
   */
  errorCode = xmlTextWriterStartDocument(writer,
					 (const char*)NULL, /* version */
					 GD_PREFERENCE_ENCODING,
					 (const char*)NULL); /* standalone */
  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Start an element named "gdisp+".
   * Since thist is the first element, this will be the root element
   * of the document.
   */
  errorCode = xmlTextWriterStartElement(writer,
					(xmlChar*)"Targa");

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Goto line and ident.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_INCREASE_INDENTATION);

  /*
   * Write a comment as child of the root element.
   * Please observe, that the input to the xmlTextWriter functions
   * HAS to be in UTF-8, even if the output XML is encoded
   * in iso-8859-1.
   */
  tmp = gdisp_xmlConvertInput("Targa General Preferences",
			      GD_PREFERENCE_ENCODING);

  errorCode = xmlTextWriterWriteComment(writer, tmp);
  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }
  if (tmp != (xmlChar*)NULL) {
    xmlFree(tmp);
  }

  /*
   * Goto line.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DO_NOT_CHANGE_INDENTATION);

  /*
   * Start an element named 'Kernel' as child of root.
   */
  errorCode = xmlTextWriterStartElement(writer,
					(xmlChar*)"Kernel");
  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Goto line and ident.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_INCREASE_INDENTATION);

  /*
   * Start an element named 'Preferences' as child of 'Kernel'.
   */
  errorCode = xmlTextWriterStartElement(writer,
					(xmlChar*)"Preferences");
  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Preferences to be saved :
   *  kernel->pathToGraphicModules
   *  kernel->sortingMethod
   *  kernel->sortingDirection
   *  kernel->dndScope
   */

  /*
   * Path to graphic modules.
   */
  attributeValue = kernel->pathToGraphicModules;
  if (attributeValue == (xmlChar*)NULL) {
    attributeValue = "unknown";
  }
  errorCode      = gdisp_xmlWriteAttributes(writer,
					    GD_INCREASE_INDENTATION,
					    indentBuffer,
					    (xmlChar*)"preference",
					    TRUE, /* end up element */
					    (xmlChar*)"pathToGraphicModules",
					    (xmlChar*)attributeValue,
					    (xmlChar*)NULL);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Sorting Method.
   */
  attributeValue = gdisp_sortingMethodToString(kernel);
  errorCode      = gdisp_xmlWriteAttributes(writer,
					    GD_DO_NOT_CHANGE_INDENTATION,
					    indentBuffer,
					    (xmlChar*)"preference",
					    TRUE, /* end up element */
					    (xmlChar*)"sortingMethod",
					    (xmlChar*)attributeValue,
					    (xmlChar*)NULL);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Sorting Direction.
   */
  attributeValue = gdisp_sortingDirectionToString(kernel);
  errorCode      = gdisp_xmlWriteAttributes(writer,
					    GD_DO_NOT_CHANGE_INDENTATION,
					    indentBuffer,
					    (xmlChar*)"preference",
					    TRUE, /* end up element */
					    (xmlChar*)"sortingDirection",
					    (xmlChar*)attributeValue,
					    (xmlChar*)NULL);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * DND Scope.
   */
  attributeValue = gdisp_dndScopeToString(kernel);
  errorCode      = gdisp_xmlWriteAttributes(writer,
					    GD_DO_NOT_CHANGE_INDENTATION,
					    indentBuffer,
					    (xmlChar*)"preference",
					    TRUE, /* end up element */
					    (xmlChar*)"dndScope",
					    (xmlChar*)attributeValue,
					    (xmlChar*)NULL);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }


  /*
   * Main board position on the screen.
   */
  sprintf(stringValue,
	  "%d,%d",
	  kernel->widgets.mainBoardWindowXPosition,
	  kernel->widgets.mainBoardWindowYPosition);
  errorCode = gdisp_xmlWriteAttributes(writer,
				       GD_DO_NOT_CHANGE_INDENTATION,
				       indentBuffer,
				       (xmlChar*)"preference",
				       TRUE, /* end up element */
				       (xmlChar*)"mainBoardPosition",
				       (xmlChar*)stringValue,
				       (xmlChar*)NULL);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }


  /*
   * Data window position on the screen.
   */
  sprintf(stringValue,
	  "%d,%d",
	  kernel->widgets.dataBookWindowXPosition,
	  kernel->widgets.dataBookWindowYPosition);
  errorCode = gdisp_xmlWriteAttributes(writer,
				       GD_DO_NOT_CHANGE_INDENTATION,
				       indentBuffer,
				       (xmlChar*)"preference",
				       TRUE, /* end up element */
				       (xmlChar*)"dataBookPosition",
				       (xmlChar*)stringValue,
				       (xmlChar*)NULL);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }


  /*
   * Host window position on the screen.
   */
  sprintf(stringValue,
	  "%d,%d",
	  kernel->widgets.hostWindowXPosition,
	  kernel->widgets.hostWindowYPosition);
  errorCode = gdisp_xmlWriteAttributes(writer,
				       GD_DO_NOT_CHANGE_INDENTATION,
				       indentBuffer,
				       (xmlChar*)"preference",
				       TRUE, /* end up element */
				       (xmlChar*)"hostPosition",
				       (xmlChar*)stringValue,
				       (xmlChar*)NULL);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }


  /*
   * Close 'Preferences' element.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DECREASE_INDENTATION);

  errorCode = xmlTextWriterEndElement(writer);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Close 'Kernel' element.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DECREASE_INDENTATION);

  errorCode = xmlTextWriterEndElement(writer);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }


  /*
   * Close 'GDisp' element.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DECREASE_INDENTATION);

  errorCode = xmlTextWriterEndElement(writer);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return;
  }

  /*
   * Goto line.
   */
  gdisp_xmlGotoLine(writer);

  /*
   * Free text writer.
   */
  xmlFreeTextWriter(writer);

  /*
   * Cleanup function for the XML library.
   */
  xmlCleanupParser();

#endif

}

