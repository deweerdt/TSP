/*
 * Created on 28 oct. 2004
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
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
