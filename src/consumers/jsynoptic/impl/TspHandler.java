/*
 * ======================== 
 * JSynoptic : a free Synoptic editor
 * ========================
 * 
 * Project Info: http://jsynoptic.sourceforge.net/index.html
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 * 
 * (C) Copyright 2001-2003, by : Corporate: Astrium SAS EADS CRC Individual:
 * Nicolas Brodu Christophe Pecquerie
 * 
 * $Id: TspHandler.java,v 1.1 2004-02-02 10:52:01 dufy Exp $
 * 
 * Changes ------- 11-Dec-2003 : Creation Date (NB);
 *  
 */
package tsp.consumer.jsynoptic.impl;

import java.io.IOException;
import java.io.Serializable;
import java.net.InetAddress;
import java.net.UnknownHostException;

import tsp.core.common.TspAnswerSample;
import tsp.core.common.TspRequestSample;
import tsp.core.common.TspSample;
import tsp.core.common.TspSampleSymbols;
import tsp.core.config.TspConfig;
import tsp.core.consumer.TspConsumer;
import tsp.core.consumer.TspSession;
import tsp.core.rpc.TSP_sample_symbol_info_list_t;
import tsp.core.rpc.TSP_sample_symbol_info_t;

public class TspHandler extends TspConsumer implements Serializable {

	public TspSampleSymbolInfo[] symbolTab_;

	private String hostName_;
	private int provider_;
	private int phase_;
	private int period_;
	private int bufferDuration_;

	private TspAnswerSample providerInfo_;
	private TspSession tspSession_;
	private int sessionId_;
	private TSP_sample_symbol_info_t[] requestedSymbolTab_;
	private int[] providerIndexTab_;
	private boolean isSampling_ = false;

	public TspHandler(String hostname, int provider)
		throws UnknownHostException, TspProviderNotFoundException {

		super();

		TspConfig.setLogLevel(TspConfig.LOG_FINER);

		hostName_ = hostname;
		provider_ = provider;

		//Setting default values
		bufferDuration_ = 100;
		period_ = 100;
		phase_ = 0;

		try {
			InetAddress.getByName(hostName_);
		} catch (UnknownHostException e) {
			throw e;
		}

		sessionId_ = openSession(hostName_, provider_);
		if (sessionId_ >= 0) {

			tspSession_ = getSession(sessionId_);
			providerInfo_ = requestInfos(sessionId_);

			//Get the symbolList from the provider
			TspSampleSymbols symbolList = new TspSampleSymbols(providerInfo_);

			//Create a new array of TspSampleSymbolsInfo
			symbolTab_ = new TspSampleSymbolInfo[symbolList.nbSymbols()];
			//Fill in the array
			for (int i = 0; i < symbolTab_.length; i++)
				symbolTab_[i] =
					new TspSampleSymbolInfo(symbolList.getSymbolByRank(i));

			providerIndexTab_ = new int[symbolList.nbSymbols()];

		} else
			throw new TspProviderNotFoundException();
	}

	public void startSampling() {
		int[] fw = { 0, 0, 0, 0 }; /* not used */
		TspRequestSample rqs =
			new TspRequestSample(
				tspSession_.answerOpen.theAnswer.version_id,
				tspSession_.answerOpen.theAnswer.channel_id,
				fw,
				1,
				new TSP_sample_symbol_info_list_t());

		rqs.setTspSSIArray(requestedSymbolTab_);
		/* send the requestSample */
		tspSession_.requestSample(rqs);
		tspSession_.requestSampleInit();
		isSampling_ = true;
	}

	public void stopSampling() {
		isSampling_ = false;
		tspSession_.requestSampleFinalize();
	}

	public int getProviderVersion() {
		return providerInfo_.theAnswer.version_id;
	}

	public double getProviderBaseFrequency() {
		return providerInfo_.theAnswer.base_frequency;
	}

	public int getProviderMaxPeriod() {
		return providerInfo_.theAnswer.max_period;
	}

	public int getProviderNbClients() {
		return providerInfo_.theAnswer.current_client_number;
	}

	public int getProviderChannelId() {
		return providerInfo_.theAnswer.channel_id;
	}

	public int getProviderNbSymbols() {
		return providerInfo_.theAnswer.symbols.value.length;
	}

	public TspSampleSymbolInfo[] getSymbolList() {
		return symbolTab_;
	}

	public TspSampleSymbolInfo getSymbol(int i) {
		return symbolTab_[i];
	}

	public String getHostname() {
		return hostName_;
	}

	public boolean isSampling() {
		return isSampling_;
	}

	public int nbSample() {
		return tspSession_.sampleFifo.nbSample();
	}

	public double getSampleValue() {
		return tspSession_.sampleFifo.getSample().value;
	}

	public TspSample getSample() {
		return tspSession_.sampleFifo.getSample();
	}

	public int getListIndex(int providerIndex) {
		return providerIndexTab_[providerIndex];
	}

	public void setSamplingPhase(int phase) {
		phase_ = phase;
	}

	public int getSamplingPhase() {
		return phase_;
	}

	public class TspProviderNotFoundException extends Exception {
		public TspProviderNotFoundException() {
			super("A TSP provider was not found on the specified host");
		}
	}

	public void close() {
		closeSession(sessionId_);
	}

	public void finalize() {
		System.out.println("Finalizing");
		close();
		try {
			super.finalize();
		} catch (Throwable e) {
			System.out.println("Erreur finalizing");
		}
	}

	/**
	 * @return Returns the bufferDuration_.
	 */
	public int getBufferDuration_() {
		return bufferDuration_;
	}

	/**
	 * @param bufferDuration_
	 *            The bufferDuration_ to set.
	 */
	public void setBufferDuration_(int bufferDuration_) {
		this.bufferDuration_ = bufferDuration_;
	}

	public TSP_sample_symbol_info_t[] createRequestSampleList() {
		int nbSample = 0;
		for (int i = 0; i < symbolTab_.length; i++) {
			if (symbolTab_[i].sample) {
				providerIndexTab_[symbolTab_[i].provider_global_index] = i;
				nbSample++;
			}
		}
		requestedSymbolTab_ = new TSP_sample_symbol_info_t[nbSample];
		int j = 0;
		for (int i = 0; i < symbolTab_.length; i++)
			if (symbolTab_[i].sample) {
				requestedSymbolTab_[j] =
					(TSP_sample_symbol_info_t) symbolTab_[i];
				requestedSymbolTab_[j].period = period_;
				requestedSymbolTab_[j].phase = phase_;
				j++;
			}
		return requestedSymbolTab_;
	}

	/**
	 * @return Returns the requestedSymbolTab_.
	 */
	public TSP_sample_symbol_info_t[] getRequestedSymbolTab_() {
		return requestedSymbolTab_;
	}

	private void writeObject(java.io.ObjectOutputStream out)
		throws IOException {
		out.writeObject(hostName_);
		out.writeInt(provider_);
		out.writeInt(period_);
		out.writeInt(phase_);
		out.writeInt(bufferDuration_);
		out.writeInt(requestedSymbolTab_.length);
		for (int i = 0; i < requestedSymbolTab_.length; i++)
			out.writeObject(requestedSymbolTab_[i].name);
	}

	private void readObject(java.io.ObjectInputStream in)
		throws IOException, ClassNotFoundException, TspProviderNotFoundException {
		
		String hostName = (String) in.readObject();
		int provider = in.readInt();
		int period = in.readInt();
		int phase = in.readInt();
		int bufferDuration = in.readInt();
		int nbSymbols = in.readInt();
		String[] requestedSymbols = new String[nbSymbols];
		for (int i=0; i<nbSymbols; i++) 
			requestedSymbols[i] = (String) in.readObject();
		
		//Construct TspHandler
		hostName_ = hostName;
		provider_ = provider;
		bufferDuration_ = bufferDuration;
		period_ = period;
		phase_ = phase;

		InetAddress.getByName(hostName_);


		sessionId_ = openSession(hostName_, provider_);
		if (sessionId_ >= 0) {

			tspSession_ = getSession(sessionId_);
			providerInfo_ = requestInfos(sessionId_);

			//Get the symbolList from the provider
			TspSampleSymbols symbolList = new TspSampleSymbols(providerInfo_);

			//Create a new array of TspSampleSymbolsInfo
			symbolTab_ = new TspSampleSymbolInfo[symbolList.nbSymbols()];
			
			//Fill in the array
			for (int i = 0; i < symbolTab_.length; i++)
				symbolTab_[i] =
					new TspSampleSymbolInfo(symbolList.getSymbolByRank(i));

			//Create a new array (TODO maybe remove?)
			providerIndexTab_ = new int[symbolList.nbSymbols()];
			
			//Find the requestedSymbols in symbolTab_ by their name
			//Set the sample flag to true for them
			boolean found;
			for(int i=0; i<requestedSymbols.length; i++) {
				found = false;
				for(int j=0; j<symbolTab_.length && !found; j++)
					if(symbolTab_[j].name.equals(requestedSymbols[i])) {
						symbolTab_[j].sample = true;
						found = true;
					}
			}
			createRequestSampleList();
			
		} else
			throw new TspProviderNotFoundException();
		
	}
	/**
	 * @return Returns the period_.
	 */
	public int getPeriod_() {
		return period_;
	}

	/**
	 * @param period_ The period_ to set.
	 */
	public void setPeriod_(int period_) {
		this.period_ = period_;
	}

	/**
	 * @return
	 */
	public String getRoundedSamplingFrequencyString(int nbFigures) {
		double samplingFrequency = getProviderBaseFrequency()/period_;
		String samplingFrequencyString = Double.toString(samplingFrequency);
		int dotIndex = samplingFrequencyString.indexOf('.');
		if(dotIndex + nbFigures < samplingFrequencyString.length() && samplingFrequency > 1)
			return samplingFrequencyString.substring(0,dotIndex + nbFigures + 1); 
		else
			return samplingFrequencyString;
	}
	
	public double getSamplingFrequency() {
		return (getProviderBaseFrequency()/period_);
	}

}
