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
 * (C) Copyright 2001-2003, by : 
 * Corporate: 
 * 				Astrium SAS 
 * 				EADS CRC 
 * Individual:
 * 				Christophe Pecquerie
 * 
 * $Id: TspHandler.java,v 1.4 2004-11-09 05:49:46 sgalles Exp $
 * 
 * Changes ------- 11-Dec-2003 : Creation Date (NB);
 *  
 */
package tsp.consumer.jsynoptic.impl;

import java.io.IOException;
import java.io.Serializable;
import java.net.UnknownHostException;

import tsp.core.common.TspAnswerSample;
import tsp.core.common.TspRequestSample;
import tsp.core.common.TspSample;
import tsp.core.common.TspSampleSymbols;
import tsp.core.common.url.TspURL;
import tsp.core.common.url.TspURLException;
import tsp.core.common.url.TspURLFactory;
import tsp.core.common.url.TspUnknownHostException;
import tsp.core.consumer.TspConsumer;
import tsp.core.consumer.TspConsumerException;
import tsp.core.consumer.TspSession;
import tsp.core.rpc.TSP_sample_symbol_info_list_t;
import tsp.core.rpc.TSP_sample_symbol_info_t;

/**
 * 
 * @author pecquerie
 * TspHandler is a handler for TSP as it is a Tsp Consumer which is always
 * connected to a provider 
 */
public class TspHandler extends TspConsumer implements Serializable {
	
	static final long serialVersionUID = -1661631201145164554L;
	
	public class TspProviderNotFoundException extends Exception {
		public TspProviderNotFoundException() {
			super("A TSP provider was not found on the specified host");
		}
	}

	private String hostName_;						//Hostname
	private int provider_;							//Provider number
	private boolean isSampling_ = false;			//Flag telling if this handler is currently sampling
	private int period_;							//Sampling period (default 100)
	private double providerBaseFrequency_;			//Base frequency of the provider
	private int phase_;								//Sampling phase (default 0)
	private int bufferDuration_;					//Buffer duration (default 100s)
	private transient int sessionId_;				//Session Id (channel ID) -1 if error
	private transient TspSession tspSession_;		//The session

	private transient TspAnswerSample providerInfo_;		//The infos about the provider	
	
	/*
	 *Array containing all the symbols to be sampled when requestSampling is launched
	 *All periods and phases are set
	 *This array is created in createRequestSampleList()
	 */
	private transient TSP_sample_symbol_info_t[] requestedSymbolTab_;

	/*
	 *Array containing all the symbols provider by this host/provider
	 *It is used in symbol selection/filtering
	 */
	private transient TspSampleSymbolInfo[] symbolTab_;

	
	/**
	 * Constructor : A TspHandler is always connected to a provider.
	 * If the provider is not found, the TspHandler should be finalized.
	 * @param hostname The hostname of the provider
	 * @param provider	The RPC provider ID
	 * @throws UnknownHostException When the hostname could not be resolved
	 * @throws TspProviderNotFoundException	When the RPC programm is not found
	 */
	public TspHandler(String hostname, int provider) throws UnknownHostException, TspProviderNotFoundException {

		super();

		//TspConfig.setLogLevel(TspConfig.LOG_FINER);

		hostName_ = hostname;
		provider_ = provider;

		//Setting default values
		bufferDuration_ = 100;
		period_ = 100;
		phase_ = 0;

		try {
			/* open Session */
			TspURL url = TspURLFactory.createEmpty();
			url.setHost(hostName_);
			url.setServerNumber(provider_);
			sessionId_ = openSession(url);

			if (sessionId_ >= 0) {

				tspSession_ = getSession(sessionId_);
				providerInfo_ = requestInfos(sessionId_);

				//Save the base frequency of provider
				providerBaseFrequency_ = providerInfo_.theAnswer.base_frequency;

				//Get the symbolList from the provider
				TspSampleSymbols symbolList = new TspSampleSymbols(providerInfo_);

				//Create a new array of TspSampleSymbolsInfo
				setSymbolTab(new TspSampleSymbolInfo[symbolList.nbSymbols()]);
				//Fill in the array
				for (int i = 0; i < getSymbolTab().length; i++)
					getSymbolTab()[i] = new TspSampleSymbolInfo(symbolList.getSymbolByRank(i));

			}
			else {
				throw new TspProviderNotFoundException();
			}

		} catch (TspConsumerException e) {
			throw new TspProviderNotFoundException();
		} catch (TspUnknownHostException e) {
			throw new UnknownHostException(e.getMessage());
		} catch (TspURLException e) {
			throw new TspProviderNotFoundException();
		}

	}
	/**
	 * Close session
	 *  
	 */
	public void close() {
		try {
			if(tspSession_ != null)
				closeSession(sessionId_);
		}
		catch (TspConsumerException e) {
			// TODO What can we do ?
			e.printStackTrace();
		}
	}

	/**
	 * Create an array of requested symbols from symbolTab_
	 * Each element has the same period/phase
	 * @return An array of TSP_sample_symbol_info_t used to request sample
	 */
	public TSP_sample_symbol_info_t[] updateRequestSampleList(String[] symbolList) {

		requestedSymbolTab_ = new TSP_sample_symbol_info_t[symbolList.length];
		
		for (int i = 0; i < getSymbolTab().length; i++) {
			if (getSymbolTab()[i].sample) {
				for(int j=0; j<symbolList.length; j++) {
					if(getSymbolTab()[i].name.equals(symbolList[j])) {
						requestedSymbolTab_[j] = (TSP_sample_symbol_info_t) getSymbolTab()[i];
						requestedSymbolTab_[j].period = period_;
						requestedSymbolTab_[j].phase = phase_;
					}						
				}
			}
		}

		return requestedSymbolTab_;
	}

	/**
	 * Finalize this Object (close session before calling gc)
	 */
	public void finalize() {
		close();
		try {
			super.finalize();
		} catch (Throwable e) {
			System.err.println("Error while finalizing TspHandler");
		}
	}
	
	/**
	 * @return Returns the buffer  duration in seconds.
	 */
	public int getBufferDuration_() {
		return bufferDuration_;
	}

	/**
	 * 
	 * @return Returns the hostname
	 */
	public String getHostname() {
		return hostName_;
	}
	
	/**
	 * @return Returns the period_.
	 */
	public int getPeriod_() {
		return period_;
	}
	
	/**
	 * 
	 * @return Returns the base frequency of the provider
	 */
	public double getProviderBaseFrequency() {
		return providerBaseFrequency_;
	}
	
	/**
	 * 
	 * @return Returns the TSP channel ID
	 */
	public int getProviderChannelId() {
		return providerInfo_.theAnswer.channel_id;
	}
	
	/**
	 * @return Returns the max period supported by the provider
	 */
	public int getProviderMaxPeriod() {
		return providerInfo_.theAnswer.max_period;
	}

	/**
	 * @return Returns the number of clients connected to the provider
	 * (maybe not the current number but rather the sum)
	 */
	public int getProviderNbClients() {
		return providerInfo_.theAnswer.current_client_number;
	}

	/**
	 * 
	 * @return Returns the number of symbols provided by the provider
	 */
	public int getProviderNbSymbols() {
		return providerInfo_.theAnswer.symbols.value.length;
	}
	
	/**
	 * 
	 * @return Returns the version of TSP of the provider
	 */
	public int getProviderVersion() {
		return providerInfo_.theAnswer.version_id;
	}
	
	/**
	 * @return Returns the requestedSymbolTab_.
	 */
	public TSP_sample_symbol_info_t[] getRequestedSymbolTab_() {
		return requestedSymbolTab_;
	}
	
	/**
	 * 
	 * @param nbFigures The number of decimals to display
	 * @return A string representing the Sampling frequency with nbFigures of decimals
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

	/**
	 * 
	 * @return Returns the first TspSample in the Fifo
	 */
	public TspSample getSample() {
		return tspSession_.sampleFifo.getSample();
	}

	/**
	 * @return Returns the value of the sample
	 */
	public double getSampleValue() {
		return tspSession_.sampleFifo.getSample().value;
	}

	/**
	 * 
	 * @return Returns the sampling frequency
	 */
	public double getSamplingFrequency() {
		return (getProviderBaseFrequency()/period_);
	}

	/**
	 * 
	 * @return Returns the sampling phase
	 */
	public int getSamplingPhase() {
		return phase_;
	}

	public void setSymbolTab(TspSampleSymbolInfo[] symbolTab_) {
		this.symbolTab_ = symbolTab_;
	}

	public TspSampleSymbolInfo[] getSymbolTab() {
		return symbolTab_;
	}

	/**
	 * @return Returns the sessionId_.
	 */
	public int getSessionId_() {
		return sessionId_;
	}

	/**
	 * 
	 * @return Returns true if this customer is sampling else returns false
	 */
	public boolean isSampling() {
		return isSampling_;
	}

	/**
	 * 
	 * @return Returns the number of samples available in the FIFO
	 */
	public int nbSample() {
		return tspSession_.sampleFifo.nbSample();
	}

	/**
	 * Provides a method to deserialize TspHandler properly
	 * @param in
	 * @throws IOException
	 * @throws ClassNotFoundException
	 * @throws TspProviderNotFoundException
	 */
	private void readObject(java.io.ObjectInputStream in)
		throws IOException, ClassNotFoundException, TspProviderNotFoundException {
		
		in.defaultReadObject();
		
		
		try {
			/* open Session */
			TspURL url = TspURLFactory.createEmpty();
			url.setHost(hostName_);
			url.setServerNumber(provider_);
			sessionId_ = openSession(url);
			if (sessionId_ >= 0) {

				tspSession_ = getSession(sessionId_);
				providerInfo_ = requestInfos(sessionId_);
			
				//Save the base frequency of provider
				providerBaseFrequency_ = providerInfo_.theAnswer.base_frequency;

				//Get the symbolList from the provider
				TspSampleSymbols symbolList = new TspSampleSymbols(providerInfo_);

				//Create a new array of TspSampleSymbolsInfo
				setSymbolTab(new TspSampleSymbolInfo[symbolList.nbSymbols()]);
			
				//Fill in the array
				for (int i = 0; i < getSymbolTab().length; i++)
					getSymbolTab()[i] =
						new TspSampleSymbolInfo(symbolList.getSymbolByRank(i));
			
			}
		}		
		catch (TspConsumerException e1) {			
			e1.printStackTrace();
			throw new TspProviderNotFoundException();
		} catch (TspURLException e) {
			e.printStackTrace();
			throw new TspProviderNotFoundException();
		}
 
	}

	/**
	 * @param bufferDuration_ The buffer duration to set in seconds
	 */
	public void setBufferDuration_(int bufferDuration_) {
		this.bufferDuration_ = bufferDuration_;
	}

	/**
	 * @param period_ The period to set.
	 */
	public void setPeriod_(int period_) {
		this.period_ = period_;
	}

	/**
	 * @param phase The phase to set.
	 */
	public void setSamplingPhase(int phase) {
		phase_ = phase;
	}

	/**
	 * Requests Samples to the Tsp provider by sending it requestedSymbolTab_
	 * Initiates sampling.
	 */
	public void startSampling() throws TspProviderNotFoundException  {
		
		try{
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
		catch (TspConsumerException e) {
			e.printStackTrace();
			throw new TspProviderNotFoundException();
		}
	}

	/**
	 * Stop sampling
	 *
	 */
	public void stopSampling() throws TspProviderNotFoundException{
		try{
			isSampling_ = false;
			tspSession_.requestSampleFinalize();
		}
		catch (TspConsumerException e) {
			e.printStackTrace();
			throw new TspProviderNotFoundException();
		}
	}

	/**
	 * Provides a method a serialize TspHandler properly
	 * @param out
	 * @throws IOException
	 */
	private void writeObject(java.io.ObjectOutputStream out)
		throws IOException {		
		out.defaultWriteObject();
	}

	/**
	 * @return Returns the provider_.
	 */
	public int getProviderId() {
		return provider_;
	}

	/**
	 * @param provider_ The provider_ to set.
	 */
	public void setProviderId(int provider_) {
		this.provider_ = provider_;
	}
	
	public String getId() {
		return	"TSP "	
		+ getHostname()
		+ " (" 
		+ getRoundedSamplingFrequencyString(2)
		+ "Hz,"
		+ getSamplingPhase()
		+ ")";
	}
	
	

}
