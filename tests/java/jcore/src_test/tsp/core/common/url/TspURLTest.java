/* $Id: TspURLTest.java,v 1.1 2004-11-09 05:49:46 sgalles Exp $
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

import tsp.core.common.url.TspSimpleURL;
import junit.framework.TestCase;


public class TspURLTest extends TestCase {

	public void testAllFields() {
		try {
			final String url = "myprotocol://myhost/myserver:1";
			TspURL o = TspURLFactory.createWithDefaultSupport(url);
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
			TspURL o = TspURLFactory.createWithDefaultSupport(url);
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
			TspURL o = TspURLFactory.createWithDefaultSupport(url);
			assertEquals("rpc", o.getProtocol());
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
			TspURL o = TspURLFactory.createWithDefaultSupport(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("localhost", o.getHost());
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
			TspURL o = TspURLFactory.createWithDefaultSupport(url);
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
		assertEquals("rpc", o.getProtocol());
		assertEquals("localhost", o.getHost());
		assertEquals(null, o.getServerName());
		assertEquals(null, o.getServerNumber());
	}

	public void testEmptyUrl() {
		try {
			assertUrlIsEmpty(TspURLFactory.createWithDefaultSupport(""));
			assertUrlIsEmpty(TspURLFactory.createWithDefaultSupport("/"));
			assertUrlIsEmpty(TspURLFactory.createWithDefaultSupport("//"));
			assertUrlIsEmpty(TspURLFactory.createWithDefaultSupport("///"));
			assertUrlIsEmpty(TspURLFactory.createWithDefaultSupport(":///"));
			assertUrlIsEmpty(TspURLFactory.createWithDefaultSupport(":///:"));
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

	public void testMixedEmpty1() {
		try {
			final String url = "myprotocol:///myserver:";
			TspURL o = TspURLFactory.createWithDefaultSupport(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("localhost", o.getHost());
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
			TspURL o = TspURLFactory.createWithDefaultSupport(url);
			assertEquals("rpc", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		}
		catch (Exception e) {
			fail("Unexpected exception : " + e);
		}
	}

}

