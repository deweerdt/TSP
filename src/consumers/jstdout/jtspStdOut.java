/* $Id: jtspStdOut.java,v 1.3 2004-11-09 05:49:46 sgalles Exp $
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

package tsp.consumer;

import java.net.UnknownHostException;

import tsp.core.consumer.*;
import tsp.core.common.*;
import tsp.core.common.url.TspURL;
import tsp.core.common.url.TspURLException;
import tsp.core.common.url.TspURLFactory;
import tsp.core.rpc.*;
import tsp.core.config.*;

class jtspStdOut {

	public static void main(String[] args) {

		try {

			TspConsumer maisPasTrop = new TspConsumer();

			/* Set up Logging level */
			TspConfig.setLogLevel(TspConfig.LOG_FINER);

			/* Initialize consumer*/
			maisPasTrop.initialize(args);

			TspURL url = TspURLFactory.createWithDefaultSupport(args[0]);
			int symbol_rank = 0;
			int nb_print = 100;			
			if (args.length > 1) {
				symbol_rank = Integer.decode(args[1]).intValue();
			}
			if (args.length > 2) {
				nb_print = Integer.decode(args[2]).intValue();
			}
						
			int sessionId = maisPasTrop.openSession(url);
			TspSession mySession = maisPasTrop.getSession(sessionId);

			/* request Infos */
			TspAnswerSample asi = maisPasTrop.requestInfos(sessionId);

			/* build request sample */
			TspSampleSymbols sampleSymbols = new TspSampleSymbols(asi);

			/* We keep all symbols before symbol_rank */
			for (int i = sampleSymbols.nbSymbols() - 1; i > symbol_rank; --i) {
				sampleSymbols.symbols.remove(i);
			}
			// 	for (int i=symbol_rank; i>0;--i) {
			// 	    sampleSymbols.getSymbolByRank(i).period=i;
			// 	}

			int[] fw = { 0, 0, 0, 0 }; /* not used */

			TspRequestSample rqs =
				new TspRequestSample(
					mySession.answerOpen.theAnswer.version_id,
					mySession.answerOpen.theAnswer.channel_id,
					fw,
					1,
					new TSP_sample_symbol_info_list_t());

			rqs.setTspSSIArray(sampleSymbols.toTspSSIArray());
			/* send the requestSample */
			mySession.requestSample(rqs);

			/* begin sampling */
			mySession.requestSampleInit();

			/* print 50 sample value */
			TspSample sample = null;
			System.out.println("Waiting first element in FIFO...");
			while (mySession.sampleFifo.nbSample() == 0) {
				System.out.print(".");
				try {
					Thread.sleep(1000);
				}
				catch (InterruptedException e) {
					System.err.println("jtspStdOut::interrupted");
				}
			}
			System.out.println("Got it go!!");

			for (int k = 0; k < nb_print; ++k) {
				if (mySession.sampleFifo.nbSample() == 0) {
					try {
						Thread.sleep(100);
					}
					catch (InterruptedException e) {
					}
				}
				sample = mySession.sampleFifo.getSample();

				System.out.println(
					"Sample <"
						+ k
						+ "> = { time_stamp ="
						+ sample.time_stamp
						+ ",  provider_global_index ="
						+ sample.provider_global_index
						+ ", value="
						+ sample.value);
			}
			/* end sampling */
			mySession.requestSampleFinalize();
			/* close Session */
			maisPasTrop.closeSession(sessionId);

		}
		catch (TspConsumerException e) {
			e.printStackTrace();
		} catch (TspURLException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}		
		

	} /* end of main */

}
