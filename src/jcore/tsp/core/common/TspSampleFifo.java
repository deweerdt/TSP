/* $Id: TspSampleFifo.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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
import java.util.LinkedList;
import java.util.NoSuchElementException;

/**
 * The TSP SampleFifo is the FIFO
 * used by the {@link TspStreamReader}
 * in order to store the {@link TspSample}.
 */
public class TspSampleFifo  {

    public TspSampleFifo(int capacity) {
	this.capacity = capacity;
	data = new LinkedList();
    }
       
    public synchronized void putSample(TspSample s) {
	data.addLast(s);
    }

    public synchronized TspSample getSample() throws NoSuchElementException {
	return (TspSample) data.removeFirst();
    }

    public synchronized int nbSample() {

	return data.size();
    }

    
    private LinkedList data;
    private int    capacity;
    
}
