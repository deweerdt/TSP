/* $Id: TspSimpleURL.java,v 1.2 2004-11-11 06:29:32 sgalles Exp $
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

package tsp.core.common.url;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * This class allow to encode/decode a Tsp URL
 */
class TspSimpleURL implements TspURL {

	private static final String SEP_PROTOCOL = "://";
	private static final String SEP_HOST = "/";
	private static final String SEP_SERVERNAME = ":";
	private static final String[] URL_REGEXP = {"([^:/]*)://([^:/]*)/([^:/]*):([0-9]*)",
																						"([^:/]*)://([^:/]*)/([^:/]*)",
																						"([^:/]*)://([^:/]*)",
																						"([^:/]*)://",
																						"([^:/]*)",
																						"/", "//", "///",																						
																						};

	private String protocol;
	private String host;
	private String serverName;
	private Integer serverNumber;

	public TspSimpleURL() {
	}

	public TspSimpleURL(String url) throws TspMalformedURLException {
		if(url == null){
			throw new TspMalformedURLException("null URL String");
		}		
		splitURL(url);
	}

	public String getURL() {
		return filterNull(getProtocol())
			+ SEP_PROTOCOL
			+ filterNull(getHost())
			+ SEP_HOST
			+ filterNull(getServerName())
			+ SEP_SERVERNAME
			+ filterNull(getServerNumber());
	}

	public String toString() {
		return getURL();
	}

	private static String filterNull(String s) {
		return s != null ? s : "";
	}

	private static String filterNull(Integer i) {
		return i != null ? i.toString() : "";
	}

	private void splitURL(String url) throws TspMalformedURLException {

		// Let's do some Functional Programming, create an array of functor
		final ValueFeeder[] feeders = { new ProtocolFeeder(), new HostFeeder(), new ServerNameFeeder(), new ServerNumberFeeder() };
		boolean isMatch = false;
		for (int r = 0; r < URL_REGEXP.length && !isMatch; r++) {
			// try a match for all known regexp
			Matcher m = Pattern.compile(URL_REGEXP[r]).matcher(url);
			if (m.matches()) {
				for (int g = 0; g < m.groupCount(); g++) {
					String currentGroup = m.group(g + 1);
					// retreive the value if the string is not empty
					if (currentGroup.length() != 0) {
						feeders[g].feed(currentGroup);
					}
				}
				isMatch = true;
			}
		}
		if (!isMatch) {
			throw new TspMalformedURLException();
		}
	}

		/**
	 * @return
	 */
	public String getHost() {
		return host;
	}

	/**
	 * @return
	 */
	public String getProtocol() {
		return protocol;
	}

	/**
	 * @return
	 */
	public String getServerName() {
		return serverName;
	}

	/**
	 * @return
	 */
	public Integer getServerNumber() {
		return serverNumber;
	}

	/**
	 * @param string
	 */
	public void setHost(String string) {
		host = string;
	}

	/**
	 * @param string
	 */
	public void setProtocol(String string) {
		protocol = string;
	}

	/**
	 * @param string
	 */
	public void setServerName(String string) {
		serverName = string;
	}

	/**
	 * @param integer
	 */
	public void setServerNumber(Integer integer) {
		serverNumber = integer;
	}
	
	public void setServerNumber(int integer) {
		serverNumber = new Integer(integer);
	}

	/**
	 * Interface used to created functors used to set the attributes of the class
	 */
	private static interface ValueFeeder {
		public void feed(String value);		
	}

	/**	 
	 * functor (as inner class) that sets the PROTOCOL
	 */
	private class ProtocolFeeder implements ValueFeeder {
		public void feed(String value) {
			setProtocol(value);
		}
		
	}

	/**	 
		 * functor (as inner class) that sets the HOST
		 */
	private class HostFeeder implements ValueFeeder {
		public void feed(String value) {
			setHost(value);
		}
		
	}

	/**	 
		 * functor (as inner class) that sets the SERVERNAME
		 */
	private class ServerNameFeeder implements ValueFeeder {
		public void feed(String value) {
			setServerName(value);
		}
		
	}

	/**	 
		 * functor (as inner class) that sets the SERVERNUMBER
		 */
	private class ServerNumberFeeder implements ValueFeeder {
		public void feed(String value) {			
				setServerNumber(new Integer(value));			
		}
		
	}
}
