/* $Id: TspStreamReader.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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

package tsp.core.common;

import tsp.core.config.*;

/**
 * The TSP StreamReader class is able to read
 * a TSP stream on a {@link DataInput} using the specified 
 * {@link TspGroups}.
 */
public class TspStreamReader implements Runnable {

    public TspStreamReader(TspDataInputStream in, TspGroup[] groups, TspSampleFifo fifo) {
	this.in     = in;
	this.groups = groups;
	this.fifo   = fifo;
	stop        = false;
    }
    
    /**
     * Read all sample symbol belonging
     * to the specified TSP group
     * @param i the number of teh TSP group to be read
     */
    public void readGroup(int i)  
	throws TspException
    {	
	TspGroup     g = groups[i];
	double       value;
	TspGroupItem item;

	/* wait to have enough data to read all group item */	      
	while (!in.available(g.encoded_size())) {		
	    try {
		Thread.sleep(10);		
	    }
	    catch (InterruptedException e) {
		TspConfig.log(TspConfig.LOG_INFO,
			      "TspStreamReader::readGroup::thread interrompu");
	    }
	}


	/* Loop over each item of the group */
	for (int rank=0; rank < g.nbItem(); ++rank) {
	    /* get item */
	    item = g.getItem(rank);
	    /* get value on the stream */
	    value = in.tspDecodeDouble();

	    /* add sample to the FIFO */
	    fifo.putSample(new TspSample(time_stamp,item.provider_global_index,value));
	    /* statistics */
	    ++sample_count;
	}      	
    }

    public void run() {
	try {
	    while (!stop) {
		
		/* we must be able to read 
		 * time_stame and group_index 
		 */	    
		while (!in.available(8)) {		    
		    try {
			Thread.sleep(10);
		    }
		    catch (InterruptedException e) {
			TspConfig.log(TspConfig.LOG_INFO,"TspStreamReader::run::thread interrompu");
		    }
		}	    
		time_stamp  = in.tspDecodeInt();
		group_index = in.tspDecodeInt();
		
		if (checkGroupIndex(group_index)) {
		    readGroup(group_index);
		}
	    } /* infinite while */
	}
	catch (TspException e) {
	    TspConfig.log(TspConfig.LOG_INFO,
			  "End of TSP stream");
	}

	TspConfig.log(TspConfig.LOG_INFO, "Thread is finishing...");
	/* bye bye guy... */
    } /* end pf run */
   

    public void stopMe() {	
	stop = true;
    }

    public boolean isStopped() {
	return stop;
    }

    private boolean checkGroupIndex(int group_index) {
	/* FIXME should check for special group case */
	if (group_index<0) {
	    TspConfig.log(TspConfig.LOG_WARNING,"Negative group index = "+group_index);
	    return false;
	}
	return true;
    }

    private boolean       stop;
    private TspDataInputStream     in;
    private TspGroup[]    groups;
    private TspSampleFifo fifo;
    private int           time_stamp;
    private int           group_index;
    /* for statistic purpose */
    private long          sample_count;

} /* end of TspStreamReader */
