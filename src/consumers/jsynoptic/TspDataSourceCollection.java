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
 *         Nicolas Brodu
 * 		   Christophe Pecquerie
 *
 * $Id: TspDataSourceCollection.java,v 1.1 2004-02-02 10:52:01 dufy Exp $
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
import tsp.consumer.jsynoptic.ui.TspDialogAddSource;
import tsp.core.rpc.TSP_sample_symbol_info_t;

public class TspDataSourceCollection
	extends DynamicDataSourceCollection
	implements ActionListener, ContextualActionProvider {

	public TspHandler tspHandler_;
	private String hostname_;
	private int provider_;
	protected SamplingThread thread;

	public TspDataSourceCollection(TspHandler tspHandler) {
		tspHandler_ = tspHandler;
	}

	/**
	 * Starts producing data automatically
	 */
	public void start() {
		tspHandler_.startSampling();
		thread = new SamplingThread();
		thread.stop = false;
		thread.start();
	}

	/**
	 * Stops producing data automatically
	 */
	public void stop() {
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
		return new DataInfo(
			"TSP "
				+ tspHandler_.getHostname()
				+ " ("
				+ tspHandler_.getRoundedSamplingFrequencyString(2)
				+ "Hz,"
				+ tspHandler_.getSamplingPhase()
				+ ")");
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
			addSource();
		}

		return false;
	}

	public void addSource() {
		TspDialogAddSource dialog = new TspDialogAddSource(tspHandler_);
		boolean dialogValidate = dialog.showDialog();
		if (dialogValidate) {
			TSP_sample_symbol_info_t[] requestedSymbolTab = tspHandler_.createRequestSampleList();
			String hostname = tspHandler_.getHostname();
			int channelID = tspHandler_.getProviderChannelId();
			//remove old sources when new ones are added
			removeAll(this);
			if (requestedSymbolTab.length != 0) {
				for (int i = 0; i < requestedSymbolTab.length; i++) {
					TSP_sample_symbol_info_t symbol = requestedSymbolTab[i];
					createDataSource(
							new DataInfo(
									symbol.name,
									symbol.name,
									"Symbolfrom " + hostname + ":" + channelID,
							"NA"),
							ValueProvider.DoubleProvider);
					try {
						int bufferSize = (int) (tspHandler_.getBufferDuration_() * tspHandler_.getSamplingFrequency());
						bufferize(
								i,
								new DelayedBuffer(ValueProvider.DoubleProvider, bufferSize));
					} catch (UnsupportedOperation e) {
						e.printStackTrace();
					}
				}
			}
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
	
	private class SamplingThread extends Thread {
		public boolean stop;
		
		public void run() {
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
