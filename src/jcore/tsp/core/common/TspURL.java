/* $Id: TspURL.java,v 1.1 2004-11-02 05:11:41 sgalles Exp $
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
 * Purpose   :Objet that handles and encode/decode TSP URL 
 * 
 * -----------------------------------------------------------------------
 */

package tsp.core.common;

/**
 * This class allow to encode/decode a Tsp URL
 */
public class TspURL {

	private static final String SEP_PROTOCOL = "://";
	private static final String SEP_HOST = "/";
	private static final String SEP_SERVERNAME = ":";
	private static final String[] EMPTY_URL = { "", "/", "//", "///", ":///" };

	private String protocol;
	private String host;
	private String serverName;
	private Integer serverNumber;

	public TspURL() {
	}

	public TspURL(String url) throws TspException {
		for (int i = 0; i < EMPTY_URL.length; i++) {
			if (url.equals(EMPTY_URL[i])) {
				return;
			}
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

	private void splitURL(String url) throws TspException {

		// Let's do some Functional Programming, create an array of functor
		final ValueFeeder[] feeders = { new ProtocolFeeder(), new HostFeeder(), new ServerNameFeeder(), new ServerNumberFeeder()};
		String remainingUrl = url;
		for (int i = 0; i < feeders.length; i++) {
			remainingUrl = feedFromUrl(remainingUrl, feeders[i]);
		}
	}

	private static String feedFromUrl(String url, ValueFeeder feeder) throws TspException {
		String token = feeder.getToken();
		String remainingUrl;
		if (token != null) {
			int lastPos = url.indexOf(token);
			if (lastPos != -1) {
				String extractedValue = url.substring(0, lastPos);
				if (extractedValue.length() != 0) {
					feeder.feed(extractedValue);
				}
				remainingUrl = url.substring(lastPos + token.length(), url.length());
			}
			else {
				throw new TspException("Malformed URL, unable to find token " + token);
			}
		}
		else {
			// null token means, take everything
			if (url.length() != 0) {
				feeder.feed(url);
			}
			remainingUrl = null;
		}
		return remainingUrl;
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

	/**
	 * Interface used to created functors used to set the attributes of the class
	 */
	private static interface ValueFeeder {
		public void feed(String value) throws TspException;
		public String getToken();
	}

	/**	 
	 * functor (as inner class) that sets the PROTOCOL
	 */
	private class ProtocolFeeder implements ValueFeeder {
		public void feed(String value) {
			setProtocol(value);
		}

		public String getToken() {
			return SEP_PROTOCOL;
		}
	}

	/**	 
		 * functor (as inner class) that sets the HOST
		 */
	private class HostFeeder implements ValueFeeder {
		public void feed(String value) {
			setHost(value);
		}
		public String getToken() {
			return SEP_HOST;
		}
	}

	/**	 
		 * functor (as inner class) that sets the SERVERNAME
		 */
	private class ServerNameFeeder implements ValueFeeder {
		public void feed(String value) {
			setServerName(value);
		}
		public String getToken() {
			return SEP_SERVERNAME;
		}
	}

	/**	 
		 * functor (as inner class) that sets the SERVERNUMBER
		 */
	private class ServerNumberFeeder implements ValueFeeder {
		public void feed(String value) throws TspException {

			try {
				setServerNumber(new Integer(value));
			}
			catch (NumberFormatException e) {
				throw new TspException("Bad URL, '" + value + "' is not an integer");
			}
		}

		public String getToken() {
			return null;
		}
	}
}
