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
 * $Id: TspDataSourceCollection.java,v 1.4 2004-11-22 07:05:04 sgalles Exp $
 * 
 * Changes ------- 06-Jan-2004 : Creation Date (NB);
 *  
 */
package tsp.consumer.jsynoptic;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import jsynoptic.base.ContextualActionProvider;
import simtools.data.DataInfo;
import simtools.data.DynamicDataSourceCollection;
import simtools.data.UnsupportedOperation;
import simtools.data.ValueProvider;
import simtools.data.buffer.DelayedBuffer;
import tsp.consumer.jsynoptic.impl.TspHandler;
import tsp.consumer.jsynoptic.impl.TspSampleSymbolInfo;
import tsp.consumer.jsynoptic.impl.TspHandler.TspProviderNotFoundException;
import tsp.consumer.jsynoptic.ui.TspDialogAddSource;
import tsp.core.rpc.TSP_sample_symbol_info_t;

public class TspDataSourceCollection
	extends DynamicDataSourceCollection
	implements ActionListener, ContextualActionProvider {

	public TspHandler tspHandler_;	
	protected SamplingThread thread;
	private DataInfo collectionInfo_; 

	public TspDataSourceCollection(TspHandler tspHandler) {
		tspHandler_ = tspHandler;
		collectionInfo_ = new DataInfo(tspHandler_.getId());
	}

	/**
	 * Starts producing data automatically
	 */
	public void start() throws TspProviderNotFoundException {
		//Create a new array containing the symbols'id
		String[] symbolList = new String[sourceInfo.length];
		for (int i=0; i< sourceInfo.length; i++) {
			symbolList[i] = sourceInfo[i].info.id;
		}
		//Pass this list to the tspHandler
		tspHandler_.updateRequestSampleList(symbolList);
		tspHandler_.startSampling();
		thread = new SamplingThread();
		thread.stop = false;
		thread.start();
	}

	/**
	 * Stops producing data automatically
	 */
	public void stop() throws TspProviderNotFoundException {
		tspHandler_.stopSampling();
		thread.stop = true;
	}

	/**
	 * Produce exactly one data in each source
	 */
	public void step() {
		long curIndex = lastIndex + 1;
		while (tspHandler_.nbSample() == 0) {
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				System.err.println("jtspStdOut::interrupted");
			}
		}
		for (int i = 0; i < size(); i++) {
			if (tspHandler_.nbSample() == 0) {
				try {
					Thread.sleep(100);
				} catch (Exception e) {
				}
			}
			setDoubleValue(i, tspHandler_.getSample().value);
		}
		registerNewValues();
	}

	public void actionPerformed(ActionEvent e) {
		step();
	}

	public DataInfo getInformation() {
		return collectionInfo_;
	}

	// -----------------------------------------------------------------------
	// ContextualActionProvide interface.
	// This adds a contextual popup menu when
	//	right-clicking on the collection in the source pane
	// -----------------------------------------------------------------------

	/**
	 * Return the list of possible actions The contect information may be used,
	 * or not.
	 * 
	 * In the case of a popup menu in the source pane, the context is
	 * SOURCELIST_CONTEXT
	 * 
	 * @param x
	 *            Coordinate, mouse position in the same unit as contains(x,y)
	 * @param y
	 *            Coordinate, mouse position in the same unit as contains(x,y)
	 * @param o
	 *            Object the actions should work on. Possibly null => default
	 *            or all actions
	 * @param context
	 *            one of the context defined in the ContextualActionProvider
	 *            class
	 * @return The list of possible actions, possibly null or an empty array
	 */
	public String[] getActions(double x, double y, Object o, int context) {

		if (context != SOURCELIST_CONTEXT)
			return null;

		// If the timer is running, we can stop it.
		// It would be possible to add a source while running, with
		// synchronize.
		// See DynamicDataSourceCollection.createDataSource(...) for details.
		// Now, let's keep this example simple, shall we?
		if (tspHandler_.isSampling())
			return new String[] { "Stop" };

		// Otherwise, we can either start the timer or add values step by step
		// - start, to start adding values automatically,
		// - step, to add only one value
		return new String[] { "Start", "Add Source" };
	}

	/**
	 * Do one of the actions previously declared by getAction.
	 * 
	 * @param x
	 *            Coordinate, for example mouse position
	 * @param y
	 *            Coordinate, for example mouse position
	 * @param o
	 *            Object the action should work on.
	 * @param action
	 *            An action returned by a previous getActions call with the
	 *            same x, y, o parameters It may be null, in which case the
	 *            default action is requested for this x,y,o.
	 * @return true if the action could be performed
	 */
	public boolean doAction(double x, double y, Object o, String action) {

		try {
			if (action.equals("Start")) {
				start();
			}
			
			if (action.equals("Step")) {
				step();
			}
			
			if (action.equals("Stop")) {
				stop();
			}
			
			if (action.equals("Add Source")) {
				addSymbols();
			}
		}
		catch (TspProviderNotFoundException e) {
			// TODO : What can we do here ?			
			e.printStackTrace();
		}
		
		return false;
	}

	//Add new sources to this Data Source Collection
	public void addSymbols() {
		//Save the old requestedSymbolTab for future comparison
		TSP_sample_symbol_info_t[] oldRequestedSymbolTab = tspHandler_.getRequestedSymbolTab_();
		//Create a dialog to add source
		TspDialogAddSource dialog = new TspDialogAddSource(tspHandler_);
		//Show it
		boolean dialogValidate = dialog.showDialog();
		//If the user clicked on the OK button
		if (dialogValidate) {
			
			TspSampleSymbolInfo[] symbolTab = tspHandler_.getSymbolTab();
			//Create the new sources list
			String hostname = tspHandler_.getUrl().getHost();
			int channelID = tspHandler_.getProviderChannelId();
			
			//Remove all removed sources
			if(oldRequestedSymbolTab != null)
			for(int i=0; i<oldRequestedSymbolTab.length; i++)
				if(!symbolTab[oldRequestedSymbolTab[i].provider_global_index].sample)
					removeDataSource(get(oldRequestedSymbolTab[i].name));
			
			//Add new symbols to the sources Vector
			for (int i = 0; i < symbolTab.length; i++) {
				if(symbolTab[i].sample) {
					TspSampleSymbolInfo symbol = symbolTab[i];
					if(get(symbol.name) == null) {
						addSource(new DataInfo(symbol.name,symbol.name,"Symbol from " + hostname + ":" + channelID,"NA"));
					}
				}
			}
		}
	}
	
	public void addSource(DataInfo di) {
		createDataSource(di,ValueProvider.DoubleProvider);
		try {
			//Calculate the buffersize = bufferduration * samplingFrequency
			int bufferSize = (int) (tspHandler_.getBufferDuration_() * tspHandler_.getSamplingFrequency());
			//Create a delayed buffer for this source
			bufferize(size()-1,new DelayedBuffer(ValueProvider.DoubleProvider, bufferSize));
		} catch (UnsupportedOperation e) {
			e.printStackTrace();
		}
	}

	/**
	 * Returns true if, and only if, it is possible to do the action right now
	 * 
	 * @param x
	 *            Coordinate, for example mouse position
	 * @param y
	 *            Coordinate, for example mouse position
	 * @param o
	 *            Object the action should work on.
	 * @param action
	 *            An action returned by a previous getActions call with the
	 *            same x, y, o parameters It may be null, in which case the
	 *            default action is requested for this x,y,o.
	 * @param context
	 *            one of the context defined in the ContextualActionProvider
	 *            class
	 * @return true if the action can be performed
	 */
	public boolean canDoAction(
		double x,
		double y,
		Object o,
		String action,
		int context) {

		// This example object is always ready to perform the actions it
		// declared
		return true;
	}
	
	/**
	 * 
	 * @author pecquerie
	 *
	 * Thread which waits for new data from the Tsp Provider
	 * then add it to the DataPool with the step method
	 */
	private class SamplingThread extends Thread {
		public boolean stop;
		
		public void run() {
			//The time to sleep between gets
			//It depends on the frequency the provider provides samples
			int timeToSleep = (int) (1000/tspHandler_.getSamplingFrequency());
			while (!stop) {
				try {
					if (tspHandler_.nbSample() != 0)
						step();
					else
						sleep(timeToSleep);
				} catch (InterruptedException e) {
					System.out.println("Interrupted...");
				}
			}
		}
	}

}
