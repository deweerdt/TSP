/* $Id: TspSession.java,v 1.6 2004-11-14 17:16:36 sgalles Exp $
 * -----------------------------------------------------------------------
 * 
 * TSP Library - core components for a generic Transport Sampling Protocol.
 * 
 * Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * -----------------------------------------------------------------------
 * 
 * Project    : TSP
 * SubProject : jTSP
 * Maintainer : tsp@astrium-space.com
 * Component  : Consumer
 * 
 * -----------------------------------------------------------------------
 * 
 * Purpose   : 
 * 
 * -----------------------------------------------------------------------
 */

package tsp.core.consumer;

import tsp.core.common.*;
import tsp.core.common.url.TspUnknownProtocolException;
import tsp.core.common.url.TspURL;
import tsp.core.common.url.TspURLException;
import tsp.core.common.url.TspUnknownHostException;
import tsp.core.rpc.*;

import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.io.IOException;

import tsp.core.config.*;

/**
 * The TSP Session class represents a TSP session
 * between a TSP consumer and a TSP provider.
 * Since TSP is n to n protocol. A TSP Consumer
 * may handle several sessions.
 * A session object contains all the information needed
 * to operate a Session.
 */
public class TspSession {

	/**
	 * The session status describes the different
	 * state of a TSP session.
	 * SAMPLE_FINALIZE= sampling process is being stopped.
	 */
	public interface TspSessionStatus {
		/**
		 * Any kind of error occured,
		 * the session may be unusable
		 */
		public static final int BROKEN = -10;
		/**
		 * Default state of a newly created session.
		 */
		public static final int IDLE = 0;
		/**
		 * the TSP request sender has been 
		 * properly created/initialized and 
		 * is ready to send TSP request.
		 */
		public static final int SENDER_READY = 10;
		/**
		 * The TSP request Open has been sent
		 * successfully the answerOpen member is 
		 * meaningfull.
		 */
		public static final int OPENED = 20;
		/**
		 * One or several TSP requests sample have been
		 * sent successfully but the sampling
		 * process has not been initialized.
		 */
		public static final int SAMPLE_CONF = 30;
		/**
		 * The TSP request sample init request has
		 * been sent, the session should go automatically
		 * to the sample receive state
		 */
		public static final int SAMPLE_INIT = 40;
		/**
		 * Sample symbols are beeing received
		 * by the TspDataStreamReader 
		 * from the provider, TspDataStreamWriter.
		 */
		public static final int SAMPLE_RECEIVE = 50;
		/**
		 * Sampling process is being stopped.
		 * The TSP session should go automatically
		 * to the SAMPLE_CONF state (TBC).
		 */
		public static final int SAMPLE_FINALIZE = 60;
	}

	/**
	 * Construct an empty TspSession
	 */
	public TspSession() {

		this.state = TspSessionStatus.IDLE;
	}

	/**
	 * The state of the TSP session.
	 * @return the state of the session.
	 */
	public int getState() {
		return state;
	}

	/**
	 * Open a TSP session between a consumer
	 * and a TSP provider.
	 * @param hostname the FQN host name where the TSP provider is up and running.
	 * @param tspProgramId the TSP provider Id. A TSP provider has an associated
	 *        Id in order to be able to run several provider on the same host.
	 * @throws TspURLException
	 * @throws TspConsumerException
	 */
	public int open(TspURL url) throws TspURLException, TspConsumerException {

		try {
			
						
			/* TODO : create a factory depending on the protocol in order to obtain a request sender */
			if(url.getProtocol() == null){
				throw new TspUnknownProtocolException("No protocol specified");
			}
			if(!url.getProtocol().equals("rpc")){
				throw new TspUnknownProtocolException("Unknown protocol : "  + url.getProtocol());
			}
											
			requestSender = new TspRequestSender(url);
								
			state = TspSessionStatus.SENDER_READY;
			/* Build the TSP request open */
			TSP_request_open_t reqO = new TSP_request_open_t();
			reqO.version_id = TspConfig.VERSION;
			reqO.argv = new TSP_argv_t();
			reqO.argv.value = null;
			reqO.argv.value = new TSP_argv_item_t[0];
			/* Send the request open to RPC server to open the session */
			TSP_answer_open_t ansO = new TSP_answer_open_t();

			ansO = requestSender.open(reqO);

			answerOpen = new TspAnswerOpen(ansO);

			state = TspSessionStatus.OPENED;
			return answerOpen.theAnswer.channel_id;		
		} catch (TspCommandChannelException e) {
			throw new TspConsumerException(e);
		}
	}

	/**
	 * Close the TSP session.
	 */
	public void close() throws TspConsumerException {

		try {
			if (state == TspSessionStatus.SAMPLE_RECEIVE) {
				requestSampleFinalize();
			}
			/* FIXME stop data receiving thread properly */
			state = TspSessionStatus.IDLE;
		}
		catch (TspConsumerException e) {
			throw new TspConsumerException(e);
		}
	}

	/**
	 * Send request infos to provider if necessary.
	 * @return the AnswerSample.
	 */
	public TspAnswerSample requestInfos(TspRequestInfos req) throws TspConsumerException {

		try {
			if (answerInfos == null) {

				TSP_answer_sample_t theAnswer = requestSender.information(req.theRequest);
				answerInfos = new TspAnswerSample(theAnswer);

			}
			return answerInfos;
		}
		catch (TspCommandChannelException e) {
			throw new TspConsumerException(e);
		}
	}

	/**
	 * Send a TSP request sample on an opened session.
	 * @req the TSP request Sample
	 */
	public TspAnswerSample requestSample(TspRequestSample req) throws TspConsumerException {

		try {
			TSP_answer_sample_t theAnswer = new TSP_answer_sample_t();

			theAnswer = requestSender.sample(req.theRequest);

			state = TspSessionStatus.SAMPLE_CONF;
			/* We cache the result for future use */
			answerSample = new TspAnswerSample(theAnswer);

			TspConfig.log(TspConfig.LOG_FINE, "answerSample");
			/* build the groups */
			buildSampleGroups();
			return answerSample;
		}
		catch (TspCommandChannelException e) {
			throw new TspConsumerException(e);
		}
	}

	/**
	 * Send a TSP request sample Init on an opened session.
	 * This ask for beginning the sample process and 
	 * to open the socket for receiving sample.
	 */
	public TspAnswerSampleInit requestSampleInit() throws TspConsumerException {

		try {
			TSP_request_sample_init_t reqSI = new TSP_request_sample_init_t();
			TSP_answer_sample_init_t theAnswer = new TSP_answer_sample_init_t();
			reqSI.version_id = answerOpen.theAnswer.version_id;
			reqSI.channel_id = answerOpen.theAnswer.channel_id;

			theAnswer = requestSender.sampleInit(reqSI);

			state = TspSessionStatus.SAMPLE_INIT;
			TspAnswerSampleInit asi = new TspAnswerSampleInit(theAnswer);

			/* create the Sample Fifo */

			sampleFifo = new TspSampleFifo(TspConfig.TSP_FIFO_SIZE);
			TspConfig.log(TspConfig.LOG_FINE, "Sample FIFO created");
			/* Open, socket */
			tspSocket = createTspSocket(asi);
			/* create the TspStreamReader */

			tspStreamReader = new TspStreamReader(new TspDataInputStream(tspSocket), groups, sampleFifo);

			/* create Thread */
			sampleFifoThread = new Thread(tspStreamReader);
			/* run it */
			sampleFifoThread.start();

			return asi;
		}
		catch (TspCommandChannelException e) {
			throw new TspConsumerException(e);
		}
		catch (TspCommonException e) {
			throw new TspConsumerException(e);
		}
		catch (UnknownHostException e) {
			throw new TspConsumerException(e);
		}
	}

	/**
	 * 
	 *
	 */
	public TspAnswerSampleFinalize requestSampleFinalize() throws TspConsumerException {

		try {
			TSP_request_sample_destroy_t reqSF = new TSP_request_sample_destroy_t();
			TSP_answer_sample_destroy_t theAnswer = new TSP_answer_sample_destroy_t();
			reqSF.version_id = answerOpen.theAnswer.version_id;
			reqSF.channel_id = answerOpen.theAnswer.channel_id;

			theAnswer = requestSender.sampleFinalize(reqSF);

			state = TspSessionStatus.SAMPLE_CONF;

			/* stop the stream reader thread properly */
			tspStreamReader.stopMe();
			/* wait for the thread death */
			try {
				sampleFifoThread.join(1000);
			}
			catch (Exception e) {
			} /* FIXME que faire quand un thread est interrompu?*/
			/* close the socket */

			tspSocket.close();

			return new TspAnswerSampleFinalize(theAnswer);
		}
		catch (TspCommandChannelException e) {
			throw new TspConsumerException(e);
		}
		catch (IOException e) {
			throw new TspConsumerException(e);
		}
	}

	/**
	 * The TSP request handler of the session
	 */
	public TspRequestSender requestSender;

	/**
	 * The session identifier obtained after
	 * a successfull TSP request open has been
	 * sent by the request sender. And is a part of 
	 * answerOpen object.
	 * The session id is the channel id of the TSP
	 * protocol which is used in subsequent TSP request.
	 */
	public TspAnswerOpen answerOpen;

	/**
	 * The description of the groups
	 */
	public TspGroup[] groups;

	/**
	 * The sample FIFO.
	 * All the method of the class implementing the FIFO
	 * are synchronized since the Fifo is read (get) by the
	 * final TSP consumer application and written by 
	 * the Tsp Stream Reader Thread.
	 */
	public TspSampleFifo sampleFifo;

	/**
	 * Build the sample group with  the last answerSample
	 */
	private void buildSampleGroups() {
		/* FIXME verify the session state */
		if (answerSample != null) {
			/* Create as many groups as specified by the 
			 * provider in the answer sample
			 */
			groups = new TspGroup[answerSample.theAnswer.provider_group_number];
			for (int j = 0; j < groups.length; ++j) {
				groups[j] = new TspGroup();
			}
			/*
			 * Build simple Sample Symbol interface 
			 */
			TspSampleSymbols tspss = new TspSampleSymbols(answerSample);
			/*
			 * Loop over symbols 
			 */
			TspConfig.log(
				TspConfig.LOG_FINE,
				"TspSession::buildSampleGroups:" + "Building sample groups for <" + tspss.nbSymbols() + "> symbols in <" + groups.length + "> groups.");
			TSP_sample_symbol_info_t s;
			TspGroupItem item;
			for (int i = 0; i < tspss.nbSymbols(); ++i) {
				/* retrieve symbol infos */
				s = (TSP_sample_symbol_info_t) tspss.symbols.get(i);
				/* create grou p item */
				item = new TspGroupItem(s.provider_global_index, tsp.core.config.TspConfig.SIZE_OF_ENCODED_DOUBLE, s.provider_group_rank);
				/* add to appropriate group */
				groups[s.provider_group_index].addItem(item);
			} /* end loop over symbols */
			/* Sort the groups:
			 * i.e. each item of one group should appear in the 
			 * lowest group rank first.
			 */
			for (int j = 0; j < groups.length; ++j) {
				groups[j].sortGroup();
			}
		} /* end if answerSample != null */
		/** FIXME should raise exception but should NEVER happen
		 ** since its a private method.
		 **/
	}

	/**
	 * Create (and connect) 
	 * the TSP socket using the answer sample init infos.
	 */
	private Socket createTspSocket(TspAnswerSampleInit asi) throws UnknownHostException, TspConsumerException {

		//JDK 1.4 String [] ap = asi.theAnswer.data_address.split(":");

		String[] ap = new String[2];
		int iodp = asi.theAnswer.data_address.indexOf(':');
		ap[0] = asi.theAnswer.data_address.substring(0, iodp);
		ap[1] = asi.theAnswer.data_address.substring(iodp + 1);

		Socket sock = null;

		try {
			sock = new Socket(InetAddress.getByName(ap[0]), (new Integer(ap[1])).intValue());
			/* Configure socket */
			sock.setReceiveBufferSize(tsp.core.config.TspConfig.DATA_STREAM_SOCKET_FIFO_SIZE / 2);
			//sock.setReuseAddress(true); FIXME JDK.1.2.2
			//sock.setKeepAlive(false);  FIXME JDK.1.2.2
			sock.setTcpNoDelay(true);
			/* 50 ms timeout on read 
			 * FIXME how to set that ???
			 */
			sock.setSoTimeout(500);
			TspConfig.log(TspConfig.LOG_INFO, "Socket connected.");

			return sock;
		}
		catch (NumberFormatException e) {
			throw new TspConsumerException(e);
		}
		catch (SocketException e) {
			throw new TspConsumerException(e);
		}
		catch (IOException e) {
			throw new TspConsumerException(e);
		}

	}

	/**
	 * the Session state
	 * (Opened, SampleReceive, SampleFinalize...)
	 */
	private int state;

	/**
	 * the TSP answer sample obtained using
	 * the TSP request infos
	 */
	private TspAnswerSample answerInfos;
	/**
	 * the TSP answer sample obtained using
	 * the TSP request sample
	 */
	private TspAnswerSample answerSample;

	/**
	 * The socket used to read the TSP sample stream.
	 */
	private Socket tspSocket;

	/**
	 * The TSP stream Reader.
	 */
	private TspStreamReader tspStreamReader;

	/**
	 * The Sample FIFO thread
	 */
	private Thread sampleFifoThread;
}

// End of TspSession.java
