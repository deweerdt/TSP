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

public class TspRequestOpen extends TspRequest {

    public TSP_request_open_t theRequest;

    
    public TspRequestOpen(TSP_request_open_t request) {
	theRequest = request;
    }

    public TspRequestOpen(int tspVersion) {
	theRequest = new TSP_request_open_t();
	theRequest.version_id = tspVersion;
    }

    public TspRequestOpen(int tspVersion, TSP_argv_t argv) {
	theRequest = new TSP_request_open_t();
	theRequest.version_id = tspVersion;
	theRequest.argv       = argv;
    }
   
}
// End of TspRequestOpen.java
