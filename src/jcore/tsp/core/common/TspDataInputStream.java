/* $Id: TspDataInputStream.java,v 1.3 2004-11-06 11:45:58 sgalles Exp $
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

import java.io.FilterInputStream;

import tsp.core.config.*;
import org.acplt.oncrpc.XdrTcpDecodingStream;
import java.net.Socket;
import java.io.InputStream;
import java.io.IOException;

/**
 * The TSP DataInputStream.
 */
public class TspDataInputStream implements TspDecodingStream {

    public TspDataInputStream(Socket streamingSocket) throws TspCommonException
    {
		try {
			tspStream    = streamingSocket.getInputStream();
			bufSize      = 1024;
			buffer       = new byte[bufSize];
			bufReadIndex = 0;
		}
		catch (IOException e) {
			throw new TspCommonException(e);
		}
    }

    /**
     * Verify if the stream may be able to read
     * the specified amount of byte.
     */
    public synchronized boolean available(int howmany) throws TspCommonException	
    {	
	if (howmany > bufSize) {
	    TspConfig.log(TspConfig.LOG_SEVERE,
			  "TspDataInputStream::available CANNOT ensure "+
			  howmany + "data in buffer of size " + bufSize);
	    /* FIXME throw exception */
	    return false;			  
	}

	/* we must read more on stream */
	if (this.available < howmany) {
	    getData();
	}
	return (this.available >= howmany);
    } /* end of available */

    protected synchronized void getData()
	throws TspCommonException
    {
	try {
	    /* move all data at the beginning of the buffer */
	    if (bufReadIndex != 0) {
		System.arraycopy(buffer,bufReadIndex,buffer,0,available);
		bufReadIndex = 0;
	    }
	    int n = tspStream.read(buffer,available,bufSize-available);
	    if (n<0) {
		throw new TspCommonException("End of TSP stream");
	    }
	    else {
		available += n;
	    }
	}
	catch (IOException e) {
	    /*TODO Dirty Fix
	    TspConfig.log(TspConfig.LOG_SEVERE,
			  "TspDataInputStream::getData IOException");*/
	}
    }


    public synchronized double tspDecodeDouble() throws TspCommonException 
		
    {	
	double a = 0.0;
	long  l;
	
	/* a TSP double is 8 byte long (64 bits) */
	while (!available(8)) {
	    available(8);
	}

	l = buffer[bufReadIndex++] & 0xFF;
	l = (l << 8) + (buffer[bufReadIndex++] & 0xFF);
	l = (l << 8) + (buffer[bufReadIndex++] & 0xFF);
	l = (l << 8) + (buffer[bufReadIndex++] & 0xFF);
	l = (l << 8) + (buffer[bufReadIndex++] & 0xFF);
	l = (l << 8) + (buffer[bufReadIndex++] & 0xFF);
	l = (l << 8) + (buffer[bufReadIndex++] & 0xFF);
	l = (l << 8) + (buffer[bufReadIndex++] & 0xFF);
	available -= 8;
	a = Double.longBitsToDouble(l);

	return a;
    }

    public synchronized int tspDecodeInt() 
	 throws TspCommonException
    {
	int i = 0;

	/* a TSP int is 4 byte long (32 bits) */

	while (!available(4)) {
	    available(4);
	}
	
	/* copied from XdrTcpDecodingStream::xdrDecodeInt */
	// Note: buf[...] gives a byte, which is signed. So if we
	// add it to the value (which is int), it has to be widened
	// to 32 bit, so its sign is propagated. To avoid this sign
	// madness, we have to "and" it with 0xFF, so all unwanted
	// bits are cut off after sign extension. Sigh.
	i = buffer[bufReadIndex++] & 0xFF;
	i = (i << 8) + (buffer[bufReadIndex++] & 0xFF);
	i = (i << 8) + (buffer[bufReadIndex++] & 0xFF);
	i = (i << 8) + (buffer[bufReadIndex++] & 0xFF);
	available -= 4;	
	// TspConfig.log(TspConfig.LOG_FINER,
// 		      "TspDataInputStream::tspDecodeInt:: int = "+ i);
	return i;
    }
    
    protected InputStream tspStream;
    protected byte[]      buffer;
    protected int         bufSize;
    protected int         available;
    protected int         bufReadIndex;
    
} /* end of TspDataInputStream */
