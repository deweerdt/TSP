/* $Id: TspSimpleURLTest.java,v 1.2 2004-11-11 06:29:32 sgalles Exp $
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


public class TspSimpleURLTest extends TestCase {

	public void testAllFields() throws Exception {
		
			final String url = "myprotocol://myhost/myserver:1";
			TspURL o = new TspSimpleURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		
	}

	public void testNoServerName() throws Exception{
		
			final String url = "myprotocol://myhost/:1";
			TspURL o = new TspSimpleURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		
	}

	public void testNoProtocol() throws Exception{
		
			final String url = "://myhost/myserver:1";
			TspURL o = new TspSimpleURL(url);
			assertEquals(null, o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		
	}

	public void testNoHost() throws Exception{
		
			final String url = "myprotocol:///myserver:1";
			TspURL o = new TspSimpleURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals(null, o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		
	}

	public void testNoServerNumber() throws Exception{

			final String url = "myprotocol://myhost/myserver:";
			TspURL o = new TspSimpleURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(null, o.getServerNumber());
	}

	private void assertUrlIsEmpty(TspURL o) {
		assertEquals(null, o.getProtocol());
		assertEquals(null, o.getHost());
		assertEquals(null, o.getServerName());
		assertEquals(null, o.getServerNumber());
	}

	public void testEmptyUrl() throws Exception{
		
			assertUrlIsEmpty(new TspSimpleURL(""));
			assertUrlIsEmpty(new TspSimpleURL("/"));
			assertUrlIsEmpty(new TspSimpleURL("//"));
			assertUrlIsEmpty(new TspSimpleURL("///"));
			assertUrlIsEmpty(new TspSimpleURL(":///"));
			assertUrlIsEmpty(new TspSimpleURL(":///:"));
		
	}

	public void testMixedEmpty1() throws Exception{
		
			final String url = "myprotocol:///myserver:";
			TspURL o = new TspSimpleURL(url);
			assertEquals("myprotocol", o.getProtocol());
			assertEquals(null, o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(null, o.getServerNumber());
		
	}

	public void testMixedEmpty2() throws Exception{
		
			final String url = "://myhost/:1";
			TspURL o = new TspSimpleURL(url);
			assertEquals(null, o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(new Integer(1), o.getServerNumber());
		
	}

	public void testSimplifiedSyntaxProtocolOnly() throws Exception {		
		
			final String url = "myprotocol";
			TspURL o = new TspSimpleURL(url);
			assertEquals( "myprotocol", o.getProtocol());
			assertEquals(null, o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(null, o.getServerNumber());			
	}
	
	public void testSimplifiedSyntaxProtocolAndToken()  throws Exception{		
		
			final String url = "myprotocol://";
			TspURL o = new TspSimpleURL(url);
			assertEquals( "myprotocol", o.getProtocol());
			assertEquals(null, o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(null, o.getServerNumber());			
	}

	public void testSimplifiedSyntaxProtocolHost()  throws Exception{		
		
			final String url = "myprotocol://myhost";
			TspURL o = new TspSimpleURL(url);
			assertEquals( "myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals(null, o.getServerName());
			assertEquals(null, o.getServerNumber());			
	}
	
	public void testSimplifiedSyntaxProtocolHostServer()  throws Exception{		
		
			final String url = "myprotocol://myhost/myserver";
			TspURL o = new TspSimpleURL(url);
			assertEquals( "myprotocol", o.getProtocol());
			assertEquals("myhost", o.getHost());
			assertEquals("myserver", o.getServerName());
			assertEquals(null, o.getServerNumber());			
	}
	
	public void testBadUrlServerNumberNotInteger() throws Exception{
		try{		
			final String url = "myprotocol://myhost/myserver:A";
			new TspSimpleURL(url);			
			fail("An exception should have been thrown");
		}
		catch (TspMalformedURLException e) {
			assertEquals(null, e.getMessage());
		}		
	}
	
	public void testBadUrlReallyWeird() throws Exception{
		try{		
			final String url = "myprotocol://myhost/myserver/1";
			new TspSimpleURL(url);			
			fail("An exception should have been thrown");
		}
		catch (TspMalformedURLException e) {
			assertEquals(null, e.getMessage());
		}		
	}
	
	public void testBadUrlNull() {
		try {			
			new TspSimpleURL(null);			
			fail("An exception should have been thrown");
		}
		catch (TspMalformedURLException e) {
			assertEquals("null URL String", e.getMessage());
		}		
	}
	
	public void testBuildURL(){
		TspURL o = new TspSimpleURL();
		o.setProtocol("myprotocol") ;
		o.setHost("myhost");
		o.setServerName("myserver");
		o.setServerNumber(new Integer(1));
		assertEquals("myprotocol://myhost/myserver:1",o.getURL());
		assertEquals("myprotocol://myhost/myserver:1",o.toString());
	}
	
	public void testBuildEmptyURL(){
			TspURL o = new TspSimpleURL();			
			assertEquals(":///:",o.getURL());
			assertEquals(":///:",o.toString());
		}
}

