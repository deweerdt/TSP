/*
 * Created on 8 nov. 2004
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package tsp.core.common.url;

import tsp.core.config.TspConfig;

/**
 * 
 */
public class TspURLFactory {

	/**
	 * Prevent instanciation 
	 */
	private TspURLFactory() {		
	}
	
	public static TspURL createWithDefaultSupport(String url) throws TspMalformedURLException{
		return new TspURLDefaultDecorator(url, TspConfig.TSP_DEFAULT_URL); 
	}
	
	public static TspURL createEmpty() throws TspMalformedURLException{
		return new TspSimpleURL(); 
	}

}
