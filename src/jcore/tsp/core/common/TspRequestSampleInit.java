/*
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

import org.acplt.oncrpc.*;
import java.io.PrintStream;
import tsp.core.rpc.*;

public class TspRequestSampleInit extends TspRequest {

    public TSP_request_sample_init_t theRequest;   
    
    public TspRequestSampleInit(TSP_request_sample_init_t request) {
	theRequest = request;
    }

    public TspRequestSampleInit(int version_id, int channel_id) {
	theRequest.version_id       = version_id;
	theRequest.channel_id       = channel_id;
    }

    public TspRequestSampleInit(int tspVersion) {
	theRequest.version_id = tspVersion;
    }

}
// End of TspRequestSampleInit.java
