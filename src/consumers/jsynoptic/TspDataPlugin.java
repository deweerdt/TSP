/* ========================
 * JSynoptic : a free Synoptic editor
 * ========================
 *
 * Project Info:  http://jsynoptic.sourceforge.net/index.html
 * 
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation;
 * either version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this
 * program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * (C) Copyright 2001-2003, by :
 *     Corporate: 
 *         Astrium SAS 
 *         EADS CRC
 *     Individual: 
 * 		   Christophe Pecquerie
 *
 * $Id: TspDataPlugin.java,v 1.2 2004-02-13 12:12:01 cpecquerie Exp $
 * 
 * Changes ------- 06-Jan-2004 : Creation Date (NB);
 *  
 */
package tsp.consumer.jsynoptic;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

import jsynoptic.base.Plugin;
import jsynoptic.ui.JSynoptic;
import simtools.data.DataInfo;
import simtools.data.DataSource;
import simtools.data.DataSourceCollection;
import simtools.data.DataSourcePool;
import simtools.data.DataSourceProvider;
import simtools.data.DuplicateIdException;
import tsp.consumer.jsynoptic.impl.TspHandler;
import tsp.consumer.jsynoptic.impl.TspSampleSymbolInfo;
import tsp.consumer.jsynoptic.ui.TspDialogOpenProvider;

public class TspDataPlugin extends Plugin implements DataSourceProvider, ActionListener{
	
	protected JMenuItem jmi;
	
	public TspDataPlugin() {
		DataSourcePool.global.addProvider(this);
	}

	/** 
	 * Adds a menu entry in the file menu
	 */
	public void setMenu(JMenuBar mb) {
		Component c[] = mb.getComponents();
		for (int i=0; i<c.length; ++i) {
			JMenu jm = (JMenu)c[i];
			//Search for the file menu
			if (jm.getText().equals(JSynoptic.resources.getString("fileMenu"))) {
				Component cm[] = jm.getMenuComponents();
				for (int j=0; j<cm.length; ++j) {
					JMenuItem jsm = (JMenuItem)cm[j];
					//Search for the menu item "open"
					if (jsm.getText().equals(JSynoptic.resources.getString("openMenu"))) {
						jmi = new JMenuItem("Open TSP Provider...");
						jm.add(jmi,2);
						jmi.addActionListener(this);
					}
				}
			}
		}
	}
	
	//Returns the plugin information
	public String about() {
		return "TSP sources, a TSP client plugin by Christophe Pecquerie";
	}

	/* (non-Javadoc)
	 * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	public void actionPerformed(ActionEvent arg0) {
		//Open dialog to connect to a new Tsp provider
		TspDialogOpenProvider windowOpenProvider = new TspDialogOpenProvider();
		TspHandler tspHandler;
		tspHandler = windowOpenProvider.getTspHandler();
		if (tspHandler != null)
		{
			//Create data sources collection
			TspDataSourceCollection tdsc = new TspDataSourceCollection(tspHandler);
			DataSourcePool.global.addDataSourceCollection(tdsc);
		}
	}
	
	/* (non-Javadoc)
	 * @see simtools.data.DataSourceProvider#getOptionalInformation(simtools.data.DataSource, simtools.data.DataSourceCollection)
	 */
	public Object getOptionalInformation(DataSource ds, DataSourceCollection dsc) {
		if ((dsc==null) || (!(dsc instanceof TspDataSourceCollection)))
			return null;

		TspDataSourceCollection tdsc = (TspDataSourceCollection) dsc;

		// As we have only random sources, we can easily re-create those.
		// Just storing the number of sources we have is enough
		return tdsc.tspHandler_;
	}

	/* (non-Javadoc)
	 * @see simtools.data.DataSourceProvider#provide(java.lang.String, java.lang.String, java.lang.Object, simtools.data.DataSourcePool)
	 */
	public DataSource provide(String id, String dscId, Object optionalInformation, DataSourcePool pool) {
		// Ignore requests that are not for this object
		if ((dscId==null) || (!dscId.startsWith("TSP"))) return null;
		
		// Usually, a provider may not need the pool  
		// In this example, we suppose there is only one instance, and that it was put in the
		// pool by this plugin. So, if the pool is not the global one, error
		if (pool!=DataSourcePool.global) return null;
		
		// Get the unique instance
		TspDataSourceCollection tdsc;
		try {
			//Get the optional information provided by the deserialization
			TspHandler tspHandler = (TspHandler)optionalInformation;
			//Get the corresponding collection
			tdsc = (TspDataSourceCollection)pool.getDataSourceCollectionWithId(tspHandler.getId());
			if (tdsc == null) {				
				TspDialogOpenProvider windowOpenProvider = new TspDialogOpenProvider(tspHandler);
				tspHandler = windowOpenProvider.getTspHandler();
				
				if(tspHandler == null)	return null;
				
				tdsc = new TspDataSourceCollection(tspHandler);
				DataSourcePool.global.addDataSourceCollection(tdsc);
			}
			//Find the source in tspHandler's symbols list
			for(int i=0; i<tdsc.tspHandler_.getSymbolTab().length ; i++)
				if(tdsc.tspHandler_.getSymbolTab()[i].name.equals(id)) {
					TspSampleSymbolInfo symbol = tdsc.tspHandler_.getSymbolTab()[i];
					tdsc.addSource(new DataInfo(symbol.name,symbol.name,"Symbol from " + tdsc.tspHandler_.getHostname() + ":" + tdsc.tspHandler_.getProviderChannelId(),"NA"));
					//Request this symbol to the provider
					tdsc.tspHandler_.getSymbolTab()[i].sample = true;
					break;
				}
			// Now we're confident all sources were re-created, so return the source for this id
			return tdsc.get(id);
		} catch (DuplicateIdException e) {
			e.printStackTrace();
			return null;
		}
	}
}
