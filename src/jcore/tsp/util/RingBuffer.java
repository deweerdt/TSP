/* $Id: RingBuffer.java,v 1.2 2004-11-02 05:11:41 sgalles Exp $
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
package tsp.util;

/**
 * A simple Ring Buffer in java.
 * This class implements a dynamically sized
 * ring buffer with zero-copy semantic.
 * That is all element are created at the 
 * ringbuf initialization. When using the ring buffer
 * put/get method only modify the value stored and not
 * the contents.
 */
public class RingBuffer {

    public RingBuffer(VisitCopy[] data) {
	this.data = data;
	pput = 0;
	pget = 0;
    }

    public int nbItem() {
	return ((pput-pget) + data.length) % data.length;
    }

    public int missed() {
	return missed;
    }

    public void put(VisitCopy o) {
	pput = (pput + 1) % data.length;
	if (pput != pget) {
	    data[pput].vcopy(o);
	}
	else {
	    --pput;
	    ++missed;
	}
    }
  
    public void get(VisitCopy o) {
	if (pput != pget) {
	    o.vcopy(data[pget]);	 
	}
	pget = (pget + 1) % data.length;
    }

    public int size() {
	return data.length;
    }

    private int      pput;
    private int      pget;
    private int      missed;
    private VisitCopy[] data;
}
