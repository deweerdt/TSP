/* $Id: TspAnswer.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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


import org.acplt.oncrpc.*;
import java.io.PrintStream;

/**
 * The <code>TspAnswer</code> is the base
 * class wrapper for all TspAnswer to a TspRequest.
 * Each sub-class will have a public field
 * called <code>theAnswer</code> which is the actual
 * answer which a TspRequestSender would have returned
 * upon a corresponding TspRequest.
 */
public abstract class TspAnswer {

    /**
     * Print the contents of the answer
     * on a PrintStream. This is default implementation
     * which may be overwritten in sub-classes
     */ 
    public void print(PrintStream printStream) {
	tsp.util.FieldPrinter.printField(this, printStream);
    }
               
}

// End of TspAnswer.java
