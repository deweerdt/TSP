/* $Id: TspSampleSymbols.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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

import java.io.PrintStream;
import java.util.Vector;
import tsp.core.rpc.*;

/**
 * The TspSampleSymbols handles a set of TSP symbols
 * it may be used to analyse the symbols contained
 * in a {@link tsp.core.common.TspAnswerSample} or to define the symbols requested
 * in a {@link tsp.core.common.TspRequestSample}.
 */
public class TspSampleSymbols {

    public TspSampleSymbols(TSP_sample_symbol_info_t[] tspSSIArray) {
	symbols = new Vector(tspSSIArray.length, 10);
	addArray(symbols,tspSSIArray);
    }

    public TspSampleSymbols(TSP_sample_symbol_info_list_t tspSSIL) {
	symbols = new Vector(tspSSIL.value.length, 10);
	addArray(symbols,tspSSIL.value);
    }

    public TspSampleSymbols(TspAnswerSample anAnswer) {
	symbols = new Vector(anAnswer.getTspSSIArray().length, 10);
	addArray(symbols,anAnswer.getTspSSIArray());
    }

    public TspSampleSymbols(TspRequestSample aRequest) {
	symbols = new Vector(aRequest.getTspSSIArray().length, 10);
	addArray(symbols,aRequest.getTspSSIArray());
    }

    public TSP_sample_symbol_info_t getSymbolByRank(int i) {
	return (TSP_sample_symbol_info_t) symbols.get(i);
    }

    /**
     * @return the number of symbols in the list
     */
    public int nbSymbols() {
	return symbols.size();
    }

    /**
     * @return the TSP Sample Symbol Info array
     *         which may be put in a TspRequest.
     */
    public TSP_sample_symbol_info_t[] toTspSSIArray() {
	return (TSP_sample_symbol_info_t[]) symbols.toArray(new TSP_sample_symbol_info_t[0]);
    }


    protected void addArray(Vector v, TSP_sample_symbol_info_t[] tspSSIArray) {
	for (int i=0; i<tspSSIArray.length; ++i) {
	    v.add(tspSSIArray[i]);
	}
    }

    public Vector symbols;
}
