/* $Id: TspGroupItem.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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

import java.lang.Comparable;


/**
 * The TSP group item.
 */
public class TspGroupItem implements Comparable {

    public TspGroupItem(int provider_global_index,
			int encoded_size,
			int group_rank) {
	this.provider_global_index = provider_global_index;
	this.encoded_size          = encoded_size;
	this.group_rank            = group_rank;
    }

    /*
     * This comparator induce a sort by
     * lowest group rank first
     */
    public int compareTo(Object o) {	
	/* this is greater than o */
	if (((TspGroupItem)o).group_rank > this.group_rank) {
	    return 1;
	}
	/* this is less than o */
	else if (((TspGroupItem)o).group_rank < this.group_rank) {
	    return -1;
	}
	/* this and o are equal */
	else {
	    return 0;
	}
    }

    /**
     * The provider global index.
     * (see TSP specs.).
     */
    public  int provider_global_index;
    /**
     * The size (in byte) to be read on the stream
     */
    public  int encoded_size;
    /**
     * the rank in the group 
     */
    public  int group_rank;
    /*** FIXME should add decoder mecanism 
     *** MAYBE a DataInput stream selector class?
     ***/
}
