/* $Id: TspConsumer.java,v 1.3 2004-11-09 05:49:46 sgalles Exp $
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

import java.util.HashMap;

import tsp.core.common.*;
import tsp.core.common.url.TspURL;
import tsp.core.common.url.TspURLException;
import tsp.core.config.*;

/**
 * The TSP consumer class is the high level
 * class which may be used by a TSP consumer application
 * in order to handle all aspect of the TSP
 * protocol from a consumer-side point of view.
 * A TSP consumer handles a set of TSP sessions {@see TspSession}
 * which represents a TSP consumer/provider relation.
 */
public class TspConsumer {
    
    /**
     * Do some specific TSP initialization
     * that may be provided on the command line
     * @param args the argument that may be passed
     *             to the main method of the final application.
     * @return true if initialization is OK, false otherwise.
     */
    public static boolean initialize(String [] args) {

	
	TspConfig.log(TspConfig.LOG_CONFIG,"Initializing TSP consumer");
	return true;
    }

    public TspConsumer() {	
	sessionMap = new HashMap();
    }

/**
 * Open a new Tsp Session with a TSP provider.
 * @param url the name of the host where the TSP provider resides.
 * @return the TSP session id.
 */
public int openSession(TspURL  url) throws TspURLException, TspConsumerException {

		TspConfig.log(TspConfig.LOG_FINE, "Open Session on " + url);

		TspSession mySession = new TspSession();

		int sessionId = mySession.open(url);

		sessionMap.put(new Integer(sessionId), mySession);

		return sessionId;

}

    /**
     * @return the number of Opened TSP Session for this consumer.
     */
    public int nbSession() {
	return sessionMap.size();
    }

    /**
     * @return the TspSession specified by its sessionId.
     */    
    public TspSession getSession(int tspSessionId) {
	return (TspSession) sessionMap.get(new Integer(tspSessionId));
    }

    /**
     * Send Request Infos on the specified session.
     * @param tspSessionId the Id of the TSP Session
     */
    public TspAnswerSample requestInfos(int tspSessionId) throws TspConsumerException {
	TspConfig.log(TspConfig.LOG_FINE,"Sending request infos, Session id = "+tspSessionId);
	TspSession mySession = getSession(tspSessionId);	
	TspRequestInfos  reqI = new TspRequestInfos(mySession.answerOpen.theAnswer.version_id,
						     mySession.answerOpen.theAnswer.channel_id);
	
	return mySession.requestInfos(reqI);
    }

    /**
     * Close an opened Tsp Session.    
     * @param tspSessionId the Id of the TSP Session to close.
     */
    public void closeSession(int tspSessionId) throws TspConsumerException {
	TspConfig.log(TspConfig.LOG_FINE,	"Closing Session id = "+tspSessionId);
	TspSession mySession = getSession(tspSessionId);	
	mySession.close();
	sessionMap.remove(new Integer(tspSessionId));
    }
        		       
    private HashMap sessionMap;	
}
