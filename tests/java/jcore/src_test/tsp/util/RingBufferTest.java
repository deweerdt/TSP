/* $Id: RingBufferTest.java,v 1.2 2004-11-02 05:11:41 sgalles Exp $
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
package tsp.util;

import junit.framework.TestCase;

/**
 * @author steph
 *
 * To change the template for this generated type comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class RingBufferTest extends TestCase {

	private RingBuffer rb;

	public void setUp() {

		IntVisitCopy[] visit = new IntVisitCopy[10];
		for (int i = 0; i < visit.length; i++) {
			visit[i] = new IntVisitCopy();
		}
		rb = new RingBuffer(visit);
	}

	public void testPutGetSimple() {

		rb.put(new IntVisitCopy(42));
		IntVisitCopy getted = new IntVisitCopy();
		rb.get(getted);
		
		assertEquals(42, getted.getValue());
	}

	private static class IntVisitCopy implements VisitCopy {

		private int value;

		IntVisitCopy() {
		}

		IntVisitCopy(int value) {
			this.value = value;
		}

		private int getValue() {
			return value;
		}

		void setValue(int value) {
			this.value = value;
		}

		public void vcopy(VisitCopy o) {
			setValue(((IntVisitCopy) o).getValue());
		}

	}

}
