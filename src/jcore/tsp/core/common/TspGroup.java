/* $Id: TspGroup.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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
import java.util.Vector;

/**
 * The TSP Group represents
 * the logical groups of sample symbols build by a TSP provider.
 */
public class TspGroup {

    public TspGroup() {
	items        = new Vector(5,5);
	encoded_size = 0;
	sorted       = false;
    }
    
    public int encoded_size() {
	return this.encoded_size;
    }

    public int nbItem() {
	return items.size();
    }

    public TspGroupItem getItem(int rank) {
	if (!sorted) {
	    sortGroup();
	}
	return (TspGroupItem)items.get(rank);
    }

    public void addItem(TspGroupItem item) {
       	items.add(item);
	encoded_size += item.encoded_size;
	sorted = false;
    }

    public void sortGroup() {
	if (!sorted) {
	    /* The items are sorted with the TspGroupItem::compareTo method */
	    java.util.Collections.sort(items);
	    sorted = true;
	}
    }
   
    private Vector  items;
    private int     encoded_size;
    private boolean sorted;
}
