/*
 * $Id: TspConfig.java,v 1.2 2004-11-09 05:49:46 sgalles Exp $
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
package tsp.core.config;

import java.lang.*;
import java.lang.reflect.*;
import java.io.PrintStream;

public class TspConfig {
    
    /**
     * Some constant
     */
    public static final int  VERSION  = 1;

    /**
     * Some maximum value 
     */
    public static int  MAX_CONSUMER_NUMBER = 100;
    public static int  MAX_PROVIDER_NUMBER = 20;

    /** 
     * RPC config values 
     */
    public static int  RPC_PROGID_BASE    = 0x31230010;
    public static int  RPC_CALL_TIMEOUT   = 20;

    /**
     * Socket config values
     */
    public static int DATA_STREAM_SOCKET_FIFO_SIZE = 1024*48;

    /**
     * TSP FIFO
     */
    public static int TSP_FIFO_SIZE                = 4000;

    /**
     * TSP Reserved Groups
     */
    public static int RESERVED_GROUP_ASYNCHRONE          = 0xFFFFFFFE;
    public static int RESERVED_GROUP_RECONF              = 0xFFFFFFFD;
    public static int RESERVED_GROUP_GLU_DATA_LOST       = 0xFFFFFFFC;
    public static int RESERVED_GROUP_CONSUMER_DATA_LOST  = 0xFFFFFFFB;

    /**
     * TSP Size
     */
    public static int SIZE_OF_ENCODED_DOUBLE             = 8;
    
    /**
     * Default TSP URL
     */
    public final static String TSP_DEFAULT_URL   = "rpc://localhost/:";

    /**
     * TSP Loggers
     */
    public static final int LOG_SEVERE  = 100;
    public static final int LOG_WARNING = 50;
    public static final int LOG_INFO    = 10;
    public static final int LOG_CONFIG  = 0;
    public static final int LOG_FINE    = -50;
    public static final int LOG_FINER   = -100;
    
    private static int LogLevel = LOG_INFO;

    public static void setLogLevel(int level) {
	LogLevel = level;
    }
    public static int getLogLevel() {
	return LogLevel;
    }

    private static void logLevelHeader(int level, PrintStream s) {
	java.util.Date d = new java.util.Date();
	d.setTime(System.currentTimeMillis());

	switch (level) {
	case LOG_SEVERE: s.print("TSP SEVERE["); break;
	case LOG_WARNING: s.print("TSP WARNING[");break;
	case LOG_INFO: s.print("TSP INFO[");break;
	case LOG_CONFIG: s.print("TSP CONFIG[");break;
	case LOG_FINE: s.print("TSP FINE[");break;
	case LOG_FINER: s.print("TSP FINER[");break;
	}
	s.print(d.toString());
	s.print("]");		
    }

    public static void log(int level, String msg) {

	if (level >= LogLevel) {	    
	    logLevelHeader(level,System.err);
	    System.err.println(msg);
	}
    }	  
} /* end of class TspConfig */
