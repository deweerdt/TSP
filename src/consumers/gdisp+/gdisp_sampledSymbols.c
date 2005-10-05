/*!  \file 

$Id: gdisp_sampledSymbols.c,v 1.7 2005-10-05 19:21:01 esteban Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Information / Actions upon available sampled symbols.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

#define GD_TREE_NB_COLUMNS 2
#define GD_TREE_SPACING    3
#define GD_TREE_COLUMN     0 /* the tree is at the first column */

/*
 * The the style of of node.
 * This routine is set up as a recursive callback.
 */
static void
gdisp_changeNodeStyle(GtkCTree     *cTree,
		      GtkCTreeNode *cNode,
		      gpointer      data)
{

  if (data != (gpointer)NULL)
    gtk_ctree_node_set_row_style(cTree,
				 cNode,
				 (GtkStyle*)data);

}


/*
 * Row selection callback.
 * Recursively change the style of all nodes.
 */
static void
gdisp_treeSelectRowCallback(GtkCTree     *cTree,
			    GtkCTreeNode *cNode,
			    gint          column,
			    gpointer      data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Recursively change the style of all sub-nodes.
   */
  gtk_ctree_pre_recursive(cTree,
			  cNode,
			  gdisp_changeNodeStyle,
			  (gpointer)kernel->widgets.selectedNodeStyle);

}
 

/*
 * Row unselection callback.
 * Recursively change the style of all nodes.
 */
static void
gdisp_treeUnselectRowCallback(GtkCTree     *cTree,
			      GtkCTreeNode *cNode,
			      gint          column,
			      gpointer      data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Recursively change the style of all sub-nodes.
   */
  gtk_ctree_pre_recursive(cTree,
			  cNode,
			  gdisp_changeNodeStyle,
			  (gpointer)kernel->widgets.unselectedNodeStyle);

}
 

/*
 * Expand the node that has been selected.
 */
static void
gdisp_expandNodeCallback(GtkCTree     *cTree,
			 GtkCTreeNode *cNode,
			 gpointer      data)
{

  /*
   * Nothing by now.
   */

}


/*
 * Collapse the node that has been selected.
 */
static void
gdisp_collapseNodeCallback(GtkCTree     *cTree,
			   GtkCTreeNode *cNode,
			   gpointer      data)
{

  /*
   * Nothing by now.
   */

}


/*
 * Get a child node according to its name.
 */
static GtkCTreeNode*
gdisp_getChildAccordingToItsName ( Kernel_T     *kernel,
				   GtkCTreeNode *parentNode,
				   gchar        *requestedChildName )
{

  GtkWidget    *cTree          =    (GtkWidget*)NULL;
  GtkCTreeRow  *parentNodeRow  =  (GtkCTreeRow*)NULL;
  GtkCTreeNode *parentChild    = (GtkCTreeNode*)NULL;
  GtkCTreeNode *requestedChild = (GtkCTreeNode*)NULL;
  gchar        *childName      =        (gchar*)NULL;

  /*
   * Deduce node row from node.
   */
  cTree = kernel->widgets.sampledSymbolHTree;
  parentNodeRow = GTK_CTREE_ROW(parentNode);

  /*
   * Loop over all children.
   */
  parentChild = parentNodeRow->children;

  while (requestedChild == (GtkCTreeNode*)NULL &&
	 parentChild    != (GtkCTreeNode*)NULL    ) {

    childName = (gchar*)gtk_ctree_node_get_row_data(GTK_CTREE(cTree),
						    parentChild);

    if (childName != (gchar*)NULL &&
	strcmp(childName,requestedChildName) == 0) {

      requestedChild = parentChild;

    }
    else {

      parentChild = GTK_CTREE_NODE_NEXT(parentChild);

    }

  }

  return requestedChild;

}


/*
 * Create a provider node.
 */
static GtkWidget*
gdisp_createHierarchicalTree ( Kernel_T *kernel )
{

  GtkWidget *cTree;
  gchar     *cTitles[GD_TREE_NB_COLUMNS] = { "Properties", "Information" };


  /*
   * Create the hierarchical tree.
   */
  cTree = gtk_ctree_new_with_titles(GD_TREE_NB_COLUMNS,
				    GD_TREE_COLUMN,
				    cTitles);

  /*
   * The style of the lines that link all nodes.
   * GTK_CTREE_LINES_NONE,
   * GTK_CTREE_LINES_SOLID,
   * GTK_CTREE_LINES_DOTTED,
   * GTK_CTREE_LINES_TABBED.
   */
  gtk_ctree_set_line_style(GTK_CTREE(cTree),
			   GTK_CTREE_LINES_DOTTED);

  /*
   * The style of all expanders.
   * The expander is the place where the user must clic in order to
   * expand or collapse nodes.
   * GTK_CTREE_EXPANDER_NONE,
   * GTK_CTREE_EXPANDER_SQUARE,
   * GTK_CTREE_EXPANDER_TRIANGLE,
   * GTK_CTREE_EXPANDER_CIRCULAR.
   */
  gtk_ctree_set_expander_style(GTK_CTREE(cTree),
			       GTK_CTREE_EXPANDER_SQUARE);

  /*
   * Set up button actions.
   * Left button selects, expands or collapses nodes.
   * All other do nothing.
   * GTK_BUTTON_IGNORED = 0,
   * GTK_BUTTON_SELECTS = 1 << 0,
   * GTK_BUTTON_DRAGS   = 1 << 1,
   * GTK_BUTTON_EXPANDS = 1 << 2.
   */
  gtk_clist_set_button_actions(GTK_CLIST(cTree),
			       0, /* left button   */
			       GTK_BUTTON_EXPANDS | GTK_BUTTON_SELECTS);

  gtk_clist_set_button_actions(GTK_CLIST(cTree),
			       1, /* middle button */
			       GTK_BUTTON_IGNORED);

  gtk_clist_set_button_actions(GTK_CLIST(cTree),
			       2, /* right button  */
			       GTK_BUTTON_IGNORED);


  /*
   * Connect all mandatory signals.
   */
  gtk_signal_connect (GTK_OBJECT(cTree),
		      "tree-expand",
		      gdisp_expandNodeCallback,
		      (gpointer)kernel); 

  gtk_signal_connect (GTK_OBJECT(cTree),
		      "tree-collapse",
		      gdisp_collapseNodeCallback,
		      (gpointer)kernel); 

  gtk_signal_connect (GTK_OBJECT(cTree),
		      "tree-select-row",
		      gdisp_treeSelectRowCallback,
		      (gpointer)kernel); 

  gtk_signal_connect (GTK_OBJECT(cTree),
		      "tree-unselect-row",
		      gdisp_treeUnselectRowCallback,
		      (gpointer)kernel); 

  /*
   * Show the tree.
   */
  gtk_widget_show(cTree);

  return cTree;

}


/*
 * Create a node that will contain all provider information.
 */
static void
gdisp_createProviderNode(Kernel_T      *kernel,
			 GtkWidget     *parent,
			 GtkWidget     *tree,
			 Provider_T    *provider,
			 GtkCTreeNode **providerNode,
			 GtkCTreeNode **sampledSymbolNode)
{

  gchar        *pNames [GD_TREE_NB_COLUMNS] = { (gchar*)NULL,  (gchar*)NULL };
  GtkCTreeNode *iNode                       = (GtkCTreeNode*)NULL;
  GtkCTreeNode *sNode                       = (GtkCTreeNode*)NULL;
  Pixmap_T     *pPixmap                     = (Pixmap_T*)NULL;

  /*
   * The name of the node is the URL of the provider.
   * Get back provider pixmap according to its identity.
   */
  pNames[0] = provider->pUrl->str;

  pPixmap = gdisp_getProviderIdPixmap(kernel,
				      parent,
				      provider->pIdentity);

  provider->pNode = gtk_ctree_insert_node(GTK_CTREE(tree),
					  (GtkCTreeNode*)NULL, /* parent  */
					  (GtkCTreeNode*)NULL, /* sibling */
					  pNames,
					  GD_TREE_SPACING,
					  pPixmap->pixmap,
					  pPixmap->mask,
					  pPixmap->pixmap,
					  pPixmap->mask,
					  FALSE, /* is a leave  */
					  TRUE); /* is expanded */

  gtk_ctree_node_set_row_data(GTK_CTREE(tree),
			      provider->pNode,
			      (gpointer)provider);

  gtk_ctree_node_set_selectable(GTK_CTREE(tree),
				provider->pNode,
				FALSE); /* not selectable */

  /*
   * Insert information node.
   */
  pNames[0] = "Information";

  pPixmap = gdisp_getPixmapById(kernel,
				GD_PIX_info,
				parent);

  iNode = gtk_ctree_insert_node(GTK_CTREE(tree),
				provider->pNode, /* provider is the parent */
				(GtkCTreeNode*)NULL, /* no sibling node */
				pNames,
				GD_TREE_SPACING,
				pPixmap->pixmap,
				pPixmap->mask,
				pPixmap->pixmap,
				pPixmap->mask,
				FALSE, /* is a leave  */
				FALSE); /* is expanded */

  gtk_ctree_node_set_selectable(GTK_CTREE(tree),
				iNode,
				FALSE); /* not selectable */

  /*
   * Insert sampled symbol node.
   */
  pNames[0] = "Sampled Symbols";
  sNode = gtk_ctree_insert_node(GTK_CTREE(tree),
				provider->pNode, /* provider is the parent */
				(GtkCTreeNode*)NULL, /* no sibling node */
				pNames,
				GD_TREE_SPACING,
				kernel->widgets.collapsedNodePixmap->pixmap,
				kernel->widgets.collapsedNodePixmap->mask,
				kernel->widgets.expandedNodePixmap->pixmap,
				kernel->widgets.expandedNodePixmap->mask,
				FALSE, /* is a leave  */
				FALSE); /* is expanded */

  gtk_ctree_node_set_row_data(GTK_CTREE(tree),
			      sNode,
			      (gpointer)"sAnchor");

  gtk_ctree_node_set_text(GTK_CTREE(tree),
			  sNode,
			  0, /* column */
			  pNames[0]);

  gtk_ctree_node_set_selectable(GTK_CTREE(tree),
				sNode,
				FALSE); /* not selectable */

  if (gdisp_getProviderNumber(kernel) == 1) {

    /*
     * Expand that node if there is only one provider.
     */
    gtk_ctree_expand(GTK_CTREE(tree),sNode);

  }

  /*
   * Return.
   */
  *providerNode      = provider->pNode;
  *sampledSymbolNode = sNode;

}


/*
 * Create a node that will contain all sampled symbol information.
 */
static void
gdisp_createSymbolNode(Kernel_T     *kernel,
		       GtkWidget    *tree,
		       GtkCTreeNode *sampledSymbolNode,
		       Symbol_T     *symbol)
{

  GtkCTreeNode *iNode                       = (GtkCTreeNode*)NULL;
  gchar         iInfo  [128];
  gchar        *iNames [GD_TREE_NB_COLUMNS] = { (gchar*)NULL,  (gchar*)NULL };

  /*
   * The name of the node is the name of the symbol.
   */
  iNames[0] = symbol->sInfo.name;
  symbol->sNode = gtk_ctree_insert_node(GTK_CTREE(tree),
					sampledSymbolNode,
					(GtkCTreeNode*)NULL, /* no sibling */
					iNames,
					GD_TREE_SPACING,
					(GdkPixmap*)NULL,
					(GdkBitmap*)NULL,
					(GdkPixmap*)NULL,
					(GdkBitmap*)NULL,
					FALSE, /* is a leave  */
					FALSE); /* is expanded */

  gtk_ctree_node_set_row_data(GTK_CTREE(tree),
			      symbol->sNode,
			      (gpointer)symbol);

  /*
   * Insert information node : PERIOD.
   */
  iNames[0] = "Period";
  iNames[1] = iInfo;
  sprintf(iNames[1],"%d",symbol->sInfo.period);

  iNode = gtk_ctree_insert_node(GTK_CTREE(tree),
				symbol->sNode, /* symbol node is the parent */
				(GtkCTreeNode*)NULL, /* no sibling node */
				iNames,
				GD_TREE_SPACING,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				TRUE,   /* is a leave  */
				FALSE); /* is expanded */

  gtk_ctree_node_set_selectable(GTK_CTREE(tree),
				iNode,
				FALSE); /* not selectable */


  /*
   * Insert information node : PHASE.
   */
  iNames[0] = "Phase";
  iNames[1] = iInfo;
  sprintf(iNames[1],"%d",symbol->sInfo.phase);

  iNode = gtk_ctree_insert_node(GTK_CTREE(tree),
				symbol->sNode, /* symbol node is the parent */
				(GtkCTreeNode*)NULL, /* no sibling node */
				iNames,
				GD_TREE_SPACING,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				TRUE,   /* is a leave  */
				FALSE); /* is expanded */

  gtk_ctree_node_set_selectable(GTK_CTREE(tree),
				iNode,
				FALSE); /* not selectable */


  /*
   * Insert information node : RANGE.
   */
  iNames[0] = "Range";
  iNames[1] = iInfo;
  sprintf(iNames[1],"[ n/a .. n/a ]");

  iNode = gtk_ctree_insert_node(GTK_CTREE(tree),
				symbol->sNode, /* symbol node is the parent */
				(GtkCTreeNode*)NULL, /* no sibling node */
				iNames,
				GD_TREE_SPACING,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				TRUE,   /* is a leave  */
				FALSE); /* is expanded */

  gtk_ctree_node_set_selectable(GTK_CTREE(tree),
				iNode,
				FALSE); /* not selectable */


  /*
   * Insert information node : REFERENCE.
   */
  iNames[0] = "Reference";
  iNames[1] = iInfo;
  sprintf(iNames[1],"%d",symbol->sReference);

  iNode = gtk_ctree_insert_node(GTK_CTREE(tree),
				symbol->sNode, /* symbol node is the parent */
				(GtkCTreeNode*)NULL, /* no sibling node */
				iNames,
				GD_TREE_SPACING,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				(GdkPixmap*)NULL,
				(GdkBitmap*)NULL,
				TRUE,   /* is a leave  */
				FALSE); /* is expanded */

  gtk_ctree_node_set_selectable(GTK_CTREE(tree),
				iNode,
				FALSE); /* not selectable */

  gtk_ctree_node_set_row_data(GTK_CTREE(tree),
			      iNode,
			      (gpointer)"sReference");

}


/*
 * Update an existing symbol node.
 */
static void
gdisp_updateSymbolNode ( Kernel_T  *kernel,
			 GtkWidget *cTree,
			 Symbol_T  *symbol )
{

  GtkCTreeNode *referenceNode = (GtkCTreeNode*)NULL;
  GString      *messageString =      (GString*)NULL;
  gchar         buffer[128];

  /*
   * The reference information is the only one to be refreshed.
   * Get back the reference node.
   */
  referenceNode = gdisp_getChildAccordingToItsName(kernel,
						   symbol->sNode,
						   "sReference");

  if (referenceNode == (GtkCTreeNode*)NULL) {

    /* should never happen, because I did create this node !! */
    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,
		     "%s symbol has no reference node.",
		     symbol->sInfo.name);
    kernel->outputFunc(kernel,messageString,GD_ERROR);

  }
  else {

    sprintf(buffer,"%d",symbol->sReference);
    gtk_ctree_node_set_text(GTK_CTREE(cTree),
			    referenceNode,
			    1, /* column */
			    buffer);

  }

}


/*
 * Finalise hierarchical tree.
 */
static void
gdisp_finaliseHierarchicalTree ( Kernel_T  *kernel,
				 GtkWidget *cTree )
{

  guint optimalWidth = 0;

  /*
   * Change tree global aspect.
   */
  gtk_clist_set_row_height(GTK_CLIST(cTree), /* CLIST, and not CTREE !! */
			   15);

  gtk_ctree_set_indent(GTK_CTREE(cTree),
		       16);

  /*
   * Compute first column optimal length.
   */
  optimalWidth = gtk_clist_optimal_column_width(GTK_CLIST(cTree),
						0 /* first column */);
  gtk_clist_set_column_width(GTK_CLIST(cTree),
			     0, /* first column */
			     optimalWidth);

}


/*
 * Graphically show the status of all sampled symbols.
 */
static void
gdisp_poolSampledSymbolList ( Kernel_T *kernel )
{

  GtkWidget        *cTree            =    (GtkWidget*)NULL;
  GString          *messageString    =      (GString*)NULL;
  GList            *providerItem     =        (GList*)NULL;
  Provider_T       *provider         =   (Provider_T*)NULL;
  Symbol_T         *symbol           =     (Symbol_T*)NULL;
  GtkCTreeNode     *pSymbolAnchor    = (GtkCTreeNode*)NULL;

  gint              pSampleCpt       =                   0;
  SampleList_T     *pSampleList      = (SampleList_T*)NULL;
  guint             pSampleMax       =                   0;


  /* ------------------------ PER PROVIDER ---------------------- */

  cTree = kernel->widgets.sampledSymbolHTree;

  /*
   * When removing a node, GTK main loop activate the "unselect"
   * callback, that performs a post recursive action on a node
   * that has been destroyed.
   * Avoid that shame by temporarily blocking the signal emission.
   */
  gtk_clist_freeze(GTK_CLIST(cTree));
  gtk_signal_handler_block_by_func(GTK_OBJECT(cTree),
				   gdisp_treeUnselectRowCallback,
				   (gpointer)kernel); 

  /*
   * Loop over all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider  = (Provider_T*)providerItem->data;

    /*
     * Look for symbol anchor.
     */
    pSymbolAnchor = gdisp_getChildAccordingToItsName(kernel,
						     provider->pNode,
						     "sAnchor");

    if (pSymbolAnchor == (GtkCTreeNode*)NULL) {

      /* should never happen, because I did create this node !! */
      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "%s provider has no anchor for symbols.",
		       provider->pUrl->str);
      kernel->outputFunc(kernel,messageString,GD_ERROR);

    }
    else {

      /* --------------------- PER SAMPLED SYMBOLS ---------------- */

      /*
       * Loop over all sampled symbol of the current provider.
       */
      pSampleList = &provider->pSampleList;
      pSampleMax  = pSampleList->len;

      for (pSampleCpt=0; pSampleCpt<pSampleMax; pSampleCpt++) {

	/*
	 * Get in touch with the symbol through the global index.
	 */
	symbol = &provider->pSymbolList[pSampleList->val[pSampleCpt].index];

	/*
	 * If referenced... ie, used by graphic plots...
	 */
	if (symbol->sReference > 0) {

	  /*
	   * Create the hierarchy for that symbol, if not already done.
	   */
	  if (symbol->sNode == (GtkCTreeNode*)NULL) {

	    gdisp_createSymbolNode(kernel,
				   cTree,
				   pSymbolAnchor,
				   symbol);

	  }
	  else {

	    gdisp_updateSymbolNode(kernel,
				   cTree,
				   symbol);

	  }

	} /* sReference > 0 */

	else {

	  if (symbol->sNode != (GtkCTreeNode*)NULL) {

	    gtk_ctree_remove_node(GTK_CTREE(cTree),
				  symbol->sNode);

	    symbol->sNode = (GtkCTreeNode*)NULL;

	  }

	} /* sReference == 0 */

      } /* loop over sampled symbols */

    } /* found sAnchor */

    /*
     * Next provider.
     */
    providerItem = g_list_next(providerItem);

  } /* loop over all providers */

  /*
   * Finalise.
   */
  gdisp_finaliseHierarchicalTree(kernel,
				 cTree);

  /*
   * Activate again the unselect handler.
   */
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(cTree),
				     gdisp_treeUnselectRowCallback,
				     (gpointer)kernel);
  gtk_clist_thaw(GTK_CLIST(cTree));

}



/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * These two procedures deal with sampled symbol list.
 * In order to graphically show all sampled symbols, a procedure
 * is registered in order to pool sampled symbol list.
 * The procedure is registered when the sampled symbol page is show
 * in the databook. The procedure is unregistered when other pages are
 * shown in the databook.
 */
void
gdisp_sampledSymbolTimer ( Kernel_T  *kernel,
			   gboolean   timerIsStarted )
{

#if defined(SAMPLED_SYMBOL_DEBUG)

  fprintf(stdout,
	  "Sampled Symbol Timer : %s\n",
	  timerIsStarted == TRUE ? "STARTED" : "STOPPED");
  fflush (stdout);

#endif

  switch (timerIsStarted) {

  case TRUE :

    /* refresh immediately */
    gdisp_poolSampledSymbolList(kernel);

    (*kernel->registerAction)(kernel,
			      gdisp_poolSampledSymbolList);

    break;

  case FALSE :

    /* stop refreshing thread status */
    (*kernel->unRegisterAction)(kernel,
				gdisp_poolSampledSymbolList);

    break;

  }

}


/*
 * Create GDISP+ sampled symbol list.
 */
void
gdisp_createSampledSymbolList ( Kernel_T  *kernel,
				GtkWidget *parent )
{

  GtkWidget        *sFrame           =    (GtkWidget*)NULL;
  GtkWidget        *scrolledWindow   =    (GtkWidget*)NULL;
  GtkWidget        *cTree            =    (GtkWidget*)NULL;
  GtkCTreeNode     *pNode            = (GtkCTreeNode*)NULL;
  GtkCTreeNode     *sNode            = (GtkCTreeNode*)NULL;
  GdkFont          *selectedFont     =      (GdkFont*)NULL;
  GdkFont          *unselectedFont   =      (GdkFont*)NULL;

  GList            *providerItem     =        (GList*)NULL;
  Provider_T       *provider         =   (Provider_T*)NULL;


  /* ------------------------ FRAME WITH LABEL ------------------------ */

  /*
   * Create a Frame that will contain a scrolled window for sampled symbols.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  sFrame = gtk_frame_new(" Sampled Symbols (for information only)");
  gtk_frame_set_label_align(GTK_FRAME(sFrame),0.1,0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(sFrame),GTK_SHADOW_ETCHED_IN);

  gtk_container_add(GTK_CONTAINER(parent),sFrame);
  gtk_widget_show(sFrame);


  /* ------- SCROLLED WINDOW FOR THE LIST OF SAMPLED SYMBOLS --------- */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_container_add(GTK_CONTAINER(sFrame),scrolledWindow); 
  gtk_widget_show(scrolledWindow);


  /* --------- HIERARCHICAL TREE FOR HANDLING ALL PROVIDERS --------- */

  cTree = gdisp_createHierarchicalTree(kernel);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow),
					cTree);
  kernel->widgets.sampledSymbolHTree = cTree;


  /* ------------------------ NODE PIXMAPS --------------------- */

  /*
   * Create expanded/collapsed node pixmap & mask.
   */
  kernel->widgets.expandedNodePixmap =
    gdisp_getPixmapById(kernel,
			GD_PIX_expandedNode,
			GTK_WIDGET(scrolledWindow));

  kernel->widgets.collapsedNodePixmap =
    gdisp_getPixmapById(kernel,
			GD_PIX_collapseNode,
			GTK_WIDGET(scrolledWindow));

  /* ------------------------ PER PROVIDER ---------------------- */

  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    /* ----------------- PROVIDER NODE CREATION ------------------ */

    /*
     * Create a node that will contain all provider information.
     */
    gdisp_createProviderNode(kernel,
			     scrolledWindow,
			     cTree,
			     provider,
			     &pNode,
			     &sNode);

    /*
     * Next provider.
     */
    providerItem = g_list_next(providerItem);

  }


  /*
   * Create the styles that contain the correct font
   * for node selection / unselection.
   */
#define GD_SELECTED_FONT   "-adobe-helvetica-bold-r-normal--12-*-*-*-*-*-*-*"
#define GD_UNSELECTED_FONT "-adobe-helvetica-medium-r-normal--12-*-*-*-*-*-*-*"

  selectedFont   = gdk_font_load(GD_SELECTED_FONT);
  unselectedFont = gdk_font_load(GD_UNSELECTED_FONT);

  if (selectedFont == (GdkFont*)NULL || unselectedFont == (GdkFont*)NULL) {

    if (selectedFont != (GdkFont*)NULL) {
      gdk_font_unref(selectedFont);
      selectedFont = (GdkFont*)NULL;
    }

    if (unselectedFont != (GdkFont*)NULL) {
      gdk_font_unref(unselectedFont);
      unselectedFont = (GdkFont*)NULL;
    }

  }
  else {

    kernel->widgets.selectedNodeStyle =
      gtk_style_copy(gtk_widget_get_default_style());

    kernel->widgets.selectedNodeStyle->font = selectedFont;
#if defined(GD_TREE_WITH_COLOR)
    kernel->widgets.selectedNodeStyle->fg[GTK_STATE_NORMAL] =
      kernel->colors[_CYAN_];
#endif

    kernel->widgets.unselectedNodeStyle =
      gtk_style_copy(gtk_widget_get_default_style());

    kernel->widgets.unselectedNodeStyle->font = unselectedFont;
#if defined(GD_TREE_WITH_COLOR)
    kernel->widgets.unselectedNodeStyle->fg[GTK_STATE_NORMAL] =
      kernel->colors[_BLACK_];
#endif

  }

  /*
   * Finalise.
   */
  gdisp_finaliseHierarchicalTree(kernel,
				 cTree);

}


/*
 * Destroy GDISP+ sampled symbol list.
 */
void
gdisp_destroySampledSymbolList ( Kernel_T *kernel )
{

  /*
   * No need to destroy tree specific fonts, because they
   * are destroyed during style destruction.
   */

  /*
   * Destroy tree specific styles.
   */
  gtk_style_unref(kernel->widgets.selectedNodeStyle);
  kernel->widgets.selectedNodeStyle = (GtkStyle*)NULL;

  gtk_style_unref(kernel->widgets.unselectedNodeStyle);
  kernel->widgets.unselectedNodeStyle = (GtkStyle*)NULL;

  /*
   * Destroy the tree.
   */
  kernel->widgets.expandedNodePixmap  = (Pixmap_T*)NULL;
  kernel->widgets.collapsedNodePixmap = (Pixmap_T*)NULL;
  kernel->widgets.sampledSymbolHTree  = (GtkWidget*)NULL;
 
}
