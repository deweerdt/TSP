/* $Id: TspAnswerSample.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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
import tsp.core.rpc.*;

public class TspAnswerSample  extends TspAnswer {

    public TSP_answer_sample_t theAnswer;

    public TspAnswerSample(TSP_answer_sample_t answer) {
	theAnswer = answer;
    }

    public TSP_sample_symbol_info_t[] getTspSSIArray() {
	return theAnswer.symbols.value;
    }

    /**
     * Print the contents of the answer 
     * on a PrintStream.
     */ 
    public void print(PrintStream printStream)  {
	
// 	String prefix ="";
// 	String prefix1;
// 	for (int i=0; i<tsp.util.FieldPrinter.level; ++i) {
// 	    prefix = prefix+tsp.util.FieldPrinter.tab;
// 	}       
// 	prefix1 = prefix;
// 	printStream.println(prefix1+"<class name=tsp.core.rpc.TspAnswerSample>");
// 	printStream.println(prefix+"<field name=theAnswer type=TSP_answer_sample_t>");
// 	prefix = prefix+tsp.util.FieldPrinter.tab;
// 	printStream.println(prefix+"<class name=tsp.core.rpc.TSP_sample_symbol_info_t>");
// 	printStream.println(prefix+"<field name=name value="
	
	tsp.util.FieldPrinter.printField(this, printStream);
// 	printStream.println(prefix+"</class>");	
// 	printStream.println(prefix1+"</class>");
			    
    }
}
// End of TspAnswerSample.java
