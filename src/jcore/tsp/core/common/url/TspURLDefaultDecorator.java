/* $Id: TspURLDefaultDecorator.java,v 1.1 2004-11-09 05:49:46 sgalles Exp $
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

/**
 * 
 */
 class TspURLDefaultDecorator  extends TspSimpleURL{
	

	private final TspURL defaultUrl;
	/**
	 * @throws TspMalformedURLException
	 * 
	 */
	public TspURLDefaultDecorator(String url, String defaultUrl) throws TspMalformedURLException {
		super(url);		
		this.defaultUrl = new TspSimpleURL(defaultUrl);
	}
	
	/* (non-Javadoc)
	 * @see tsp.core.common.url.TspURL#getHost()
	 */
	public String getHost() {
		String host =  super.getHost();
		return host !=null ? host : defaultUrl.getHost();
	}
	/* (non-Javadoc)
	 * @see tsp.core.common.url.TspURL#getProtocol()
	 */
	public String getProtocol() {
		String protocol =  super.getProtocol();
		return protocol !=null ? protocol : defaultUrl.getProtocol();
	}
	/* (non-Javadoc)
	 * @see tsp.core.common.url.TspURL#getServerName()
	 */
	public String getServerName() {
		String serverName =  super.getServerName();
		return serverName !=null ? serverName : defaultUrl.getServerName();
	}
	/* (non-Javadoc)
	 * @see tsp.core.common.url.TspURL#getServerNumber()
	 */
	public Integer getServerNumber() {
		Integer serverNumber =  super.getServerNumber();
		return serverNumber !=null ? serverNumber : defaultUrl.getServerNumber();
	}	

}
