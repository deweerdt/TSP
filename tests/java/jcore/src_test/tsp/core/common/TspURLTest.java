/* $Id: TspURLTest.java,v 1.1 2004-11-02 05:11:41 sgalles Exp $
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

import junit.framework.TestCase;


public class TspURLTest extends TestCase {

	public void testAllFields() {
		try {
			final String url = "myprotocol://myhost/myserver:1";
			TspURL o = new TspURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testNoServerName() {
		try {
			final String url = "myprotocol://myhost/:1";
			TspURL o = new TspURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testNoProtocol() {
		try {
			final String url = "://myhost/myserver:1";
			TspURL o = new TspURL(url);
			assertEquals(null, o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testNoHost() {
		try {
			final String url = "myprotocol:///myserver:1";
			TspURL o = new TspURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals(null, o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testNoServerNumber() {
		try {
			final String url = "myprotocol://myhost/myserver:";
			TspURL o = new TspURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(null, o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	private void assertUrlIsEmpty(TspURL o) {
		assertEquals(null, o.getProtocol());
		assertEquals(null, o.getHost());
		assertEquals(null, o.getServerName());
		assertEquals(null, o.getServerNumber());
	}

	public void testEmptyUrl() {
		try {
			assertUrlIsEmpty(new TspURL(""));
			assertUrlIsEmpty(new TspURL("/"));
			assertUrlIsEmpty(new TspURL("//"));
			assertUrlIsEmpty(new TspURL("///"));
			assertUrlIsEmpty(new TspURL(":///"));
			assertUrlIsEmpty(new TspURL(":///:"));
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testMixedEmpty1() {
		try {
			final String url = "myprotocol:///myserver:";
			TspURL o = new TspURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals(null, o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(null, o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testMixedEmpty2() {
		try {
			final String url = "://myhost/:1";
			TspURL o = new TspURL(url);
			assertEquals(null, o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testBadUrlMissingTokenProtocol() {
		try {
			final String url = ":myhost/:1";
			new TspURL(url);
			fail("A TspException should have been thrown");
		}
		catch (TspException e) {
			assertEquals("Malformed URL, unable to find token ://", e.getMessage());
		}
		catch (Exception e) {
			fail("Unexpected Exception : " + e);
		}

	}

	public void testBadUrlMissingTokenHost() {
		try {
			final String url = "://myhost1";
			new TspURL(url);
			fail("A TspException should have been thrown");
		}
		catch (TspException e) {
			assertEquals("Malformed URL, unable to find token /", e.getMessage());
		}
		catch (Exception e) {
			fail("Unexpected Exception : " + e);
		}
	}

	public void testBadUrlMissingTokenServerName() {
		try {
			final String url = "://myhost/1";
			new TspURL(url);
			fail("A TspException should have been thrown");
		}
		catch (TspException e) {
			assertEquals("Malformed URL, unable to find token :", e.getMessage());
		}
		catch (Exception e) {
			fail("Unexpected Exception : " + e);
		}
	}

	public void testBadUrlMissingServerNumberNotInteger() {
		try {
			final String url = "myprotocol://myhost/myserver:A";
			new TspURL(url);			
			fail("A TspException should have been thrown");
		}
		catch (TspException e) {
			assertEquals("Bad URL, 'A' is not an integer", e.getMessage());
		}
		catch (Exception e) {
			fail("Unexpected Exception : " + e);
		}
	}
	
	public void testBuildURL(){
		TspURL o = new TspURL();
		o.setProtocol("myprotocol") ;
		o.setHost("myhost");
		o.setServerName("myserver");
		o.setServerNumber(new Integer(1));
		assertEquals("myprotocol://myhost/myserver:1",o.getURL());
		assertEquals("myprotocol://myhost/myserver:1",o.toString());
	}
	
	public void testBuildEmptyURL(){
			TspURL o = new TspURL();			
			assertEquals(":///:",o.getURL());
			assertEquals(":///:",o.toString());
		}
}

