/* $Id: TspRequestSender.java,v 1.4 2004-11-14 17:35:22 sgalles Exp $
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

package tsp.core.rpc;

import org.acplt.oncrpc.*;

import tsp.core.common.url.TspNoServerFoundException;
import tsp.core.common.url.TspURL;
import tsp.core.common.url.TspURLException;
import tsp.core.common.url.TspUnknownHostException;
import tsp.core.common.url.TspUnknownServerNameException;
import tsp.core.common.url.TspUnknownServerNumberException;
import tsp.core.config.TspConfig;

import java.net.InetAddress;
import java.net.UnknownHostException;

import java.io.IOException;

/**
 * The TSP Request Sender class is high level
 * wrapper to a lower layer which is able to send
 * TSP request. 
 *
 */
public class TspRequestSender {

	private TSP_rpcClientStub stub;

	/**
	 * Constructs a <code>TSP_rpcClientStub</code> client stub proxy object
	 * from which the TSP_RPC remote program can be accessed.
	 * 
	 * @param host
	 *                      Internet address of host where to contact the remote program.
	 * @param progid
	 *                      The remote program number.
	 * @throws TspUrlException
	 * @throws OncRpcException
	 *                       if an ONC/RPC error occurs.
	 * @throws IOException
	 *                       if an I/O error occurs.
	 */
	public TspRequestSender(TspURL url) throws TspCommandChannelException, TspURLException {
		
		/* TODO : manage stub connection leak */
		stub = null;
		
		if (url.getServerNumber() != null) {
			int serverNumber = url.getServerNumber().intValue();
			stub = createStub(url.getHost(), serverNumber, url.getServerName());
		} else {
			boolean oneServerExists = false;
			for(int serverNumber = 0 ; serverNumber < TspConfig.TSP_MAX_SERVERNUMBER_SCAN && stub == null; serverNumber++){
				try{
					System.out.println("Searching server for serverName=" + url.getServerName() + " and serverNumber=" + serverNumber);
					stub = createStub(url.getHost(), serverNumber, url.getServerName());
				} catch (TspUnknownServerNumberException e) {
					System.out.println("No server found for serverNumber=" + serverNumber);
					/* empty on purpose, retry */
				} catch (TspCommandChannelException e) {
					System.out.println("No server found for serverNumber=" + serverNumber);
					/* empty on purpose, retry */
				} catch (TspUnknownServerNameException e) {
					/*
					 * it is not the right server, bu at least there's one
					 * server
					 */
					System.out.println("No server found for serverName=" + url.getServerName() + " and serverNumber=" + serverNumber);
					oneServerExists = true;	
				} 	
			}
			if(stub == null){
				if(oneServerExists){
					throw new TspUnknownServerNameException("Unable to find server with serverName='" + url.getServerName() + "'");
				}else{
					throw new TspNoServerFoundException("Unable to find any server with serverNumber from 0 to " + TspConfig.TSP_MAX_SERVERNUMBER_SCAN);
				}
			}
		}

	}
	
	private static TSP_rpcClientStub  createStub(String  sHost, int serverNumber) throws TspCommandChannelException, TspUnknownHostException, TspUnknownServerNumberException  {

		if(sHost == null){
			throw new TspUnknownHostException("hostName not be empty");
		}
		
		try {
			InetAddress host = InetAddress.getByName(sHost);
			TSP_rpcClientStub locStub = new TSP_rpcClientStub(host, tsp.core.config.TspConfig.RPC_PROGID_BASE + serverNumber, TSP_rpc.TSP_RPC_VERSION_INITIAL, OncRpcProtocols.ONCRPC_TCP);
			locStub.getClient().setTimeout(tsp.core.config.TspConfig.RPC_CALL_TIMEOUT * 1000);
			return locStub;
		} catch (UnknownHostException e) {
			throw new TspUnknownHostException(e);
		} catch (OncRpcProgramNotRegisteredException e) {
			throw new TspUnknownServerNumberException("serverNumber  = " + serverNumber, e);	
		} catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		} catch (IOException e) {
			throw new TspCommandChannelException(e);
		}

	}
	
	private static TSP_rpcClientStub  createStub(String  sHost, int serverNumber, String serverName) throws TspCommandChannelException, TspURLException  {
		try {
			TSP_rpcClientStub locStub = createStub(sHost, serverNumber);
			TSP_provider_info_t info = locStub.TSP_PROVIDER_INFORMATION_1();
			if(serverName != null && !info.info.equals(serverName)){
				throw new TspUnknownServerNameException("for serverNumber " + serverNumber + ", realServerName = '" +info.info + "', requestedServerName = '" +serverName + "'" );
			}
			return locStub;						
		} catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		} catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}
	

	/**
	 * Send a TSP request Open to the provider. This is the first request to
	 * send to a provider since it is necessary for other request.
	 * 
	 * @return Answer from server which contains information about protocol
	 *                 version and other necessary information for sending other request
	 *                 {@link tsp.rpc.TSP_answer_open_t}
	 * @throws OncRpcException
	 *                       if an ONC/RPC error occurs.
	 * @throws IOException
	 *                       if an I/O error occurs.
	 */
	public TSP_answer_open_t open(TSP_request_open_t req_open) throws TspCommandChannelException {
		try {
			return stub.TSP_REQUEST_OPEN_1(req_open);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

	/**
	 * Send a TSP request Close to the provider.
	 * @throws OncRpcException if an ONC/RPC error occurs.
	 * @throws IOException if an I/O error occurs.
	 */
	public void close(TSP_request_close_t req_close) throws TspCommandChannelException {

		try {
			stub.TSP_REQUEST_CLOSE_1(req_close);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

	/**
	 * Send a TSP request Information to the provider.
	 * @return a TSP answer sample which contains the list
	 *         of available symbols and other informations
	 *         regarding provider capability.
	 * @throws OncRpcException if an ONC/RPC error occurs.
	 * @throws IOException if an I/O error occurs.
	 */
	public TSP_answer_sample_t information(TSP_request_information_t req_info) throws TspCommandChannelException {

		try {
			return stub.TSP_REQUEST_INFORMATION_1(req_info);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

	/**
	 * Send a TSP request Feature to the provider.
	 * @return a TSP answer feature which specifies if
	 *         the requested feature is supported and/or activated.
	 * @throws OncRpcException if an ONC/RPC error occurs.
	 * @throws IOException if an I/O error occurs.
	 */
	public TSP_answer_feature_t feature(TSP_request_feature_t req_feature) throws TspCommandChannelException {

		try {
			return stub.TSP_REQUEST_FEATURE_1(req_feature);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

	/**
	 * Send a TSP request ExecFeature to the provider.
	 * @return a TSP answer feature which specifies if
	 *         the requested feature is supported and/or activated.
	 * @throws OncRpcException if an ONC/RPC error occurs.
	 * @throws IOException if an I/O error occurs.
	 */
	public void execFeature(TSP_exec_feature_t exec_feature) throws TspCommandChannelException {

		try {
			stub.TSP_EXEC_FEATURE_1(exec_feature);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

	/**
	 * Send a TSP request Sample to the provider.
	 * @return a TSP answer sample which specifies if
	 *         the requested sample is accepted.
	 * @throws OncRpcException if an ONC/RPC error occurs.
	 * @throws IOException if an I/O error occurs.
	 */
	public TSP_answer_sample_t sample(TSP_request_sample_t req_sample) throws TspCommandChannelException {

		try {
			return stub.TSP_REQUEST_SAMPLE_1(req_sample);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

	/**
	 * Send a TSP request Sample Init to the provider.
	 * @return a TSP answer sample Init which specifies if
	 *         the requested sample is accepted.
	 * @throws OncRpcException if an ONC/RPC error occurs.
	 * @throws IOException if an I/O error occurs.
	 */
	public TSP_answer_sample_init_t sampleInit(TSP_request_sample_init_t req_sampleInit) throws TspCommandChannelException {

		try {
			return stub.TSP_REQUEST_SAMPLE_INIT_1(req_sampleInit);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

	/**
	 * Send a TSP request Sample Finalize to the provider.
	 * @return a TSP answer sample destroy which specifies if
	 *         the request has been properly processed by the provider.
	 * @throws OncRpcException if an ONC/RPC error occurs.
	 * @throws IOException if an I/O error occurs.
	 */
	public TSP_answer_sample_destroy_t sampleFinalize(TSP_request_sample_destroy_t req_sampleDestroy) throws TspCommandChannelException {

		try {
			return stub.TSP_REQUEST_SAMPLE_DESTROY_1(req_sampleDestroy);
		}
		catch (OncRpcException e) {
			throw new TspCommandChannelException(e);
		}
		catch (IOException e) {
			throw new TspCommandChannelException(e);
		}
	}

}

// End of TspRequestSender.java
