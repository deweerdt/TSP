/* ========================
 * JSynoptic : a free Synoptic editor
 * ========================
 *
 * Project Info:  http://jsynoptic.sourceforge.net/index.html
 * 
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation;
 * either version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this
 * program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * (C) Copyright 2001-2003, by :
 *     Corporate: 
 *         Astrium SAS 
 *         EADS CRC
 *     Individual: 
 *         Nicolas Brodu
 * 		   Christophe Pecquerie
 *
 * $Id: TspSampleSymbolInfo.java,v 1.1 2004-02-02 10:52:01 dufy Exp $
 * 
 * Changes ------- 20-Jan-2004 : Creation Date (NB);
 *  
 */
package tsp.consumer.jsynoptic.impl;

import tsp.core.rpc.TSP_sample_symbol_info_t;

public class TspSampleSymbolInfo extends TSP_sample_symbol_info_t {
	
	public boolean display;
	public boolean sample;
	
	public TspSampleSymbolInfo(TSP_sample_symbol_info_t tssi) {
		super();
		name = tssi.name;
		provider_global_index = tssi.provider_global_index;
		provider_group_index = tssi.provider_group_index;
		provider_group_rank = tssi.provider_group_rank;
		xdr_tsp_t = tssi.xdr_tsp_t;
		dimension = tssi.dimension;
		period = tssi.period;
		phase = tssi.phase;
		
		display = true;
		sample = false;
	}
	
	public String toString() {
		return name;
	}
	
	
	/* (non-Javadoc)
	 * @see java.lang.Object#clone()
	 */
	public Object clone() throws CloneNotSupportedException {
		// TODO Auto-generated method stub
		return super.clone();
	}

}
