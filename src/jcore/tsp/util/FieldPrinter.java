/* $Id: FieldPrinter.java,v 1.1 2003-02-07 16:09:48 SyntDev1 Exp $
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

import java.lang.*;
import java.lang.reflect.*;
import java.io.PrintStream;

/**
 * This class use the reflection capability of Java to print
 * the data fields of an object in XML-like syntax.
 */
public class FieldPrinter {

    public static int level = -1;
    public static String tab   = "   ";

    /**
     * Print the field of the class from which the object
     * is an instance on a PrintStream.
     * BEWARE THAT THIS METHOD is implemented in recursive way
     * and is NOT RE-ENTRANT since it use static class variables.
     * 2 thread should not call this method simultaneously.
     * FIXME
     * One could alleviate the problem with a third argument
     * FIXME
     */
    public static final void printField(Object o, PrintStream printStream) {

	FieldPrinter.level = FieldPrinter.level + 1;
	String prefix = "";
	for (int j=0; j<level;++j) {
	    prefix = prefix + tab;
	}
	Class c = o.getClass();
     
	/* if we encounter a primitive type just print its value */
	if (c.isPrimitive()) {
	    printStream.print(prefix+"<"+c.getName()+" value=");
	    printStream.print(o);
	    printStream.println("/>");            
	}
	else if (c.isArray()) {
	    for (int i=0; i<Array.getLength(o);++i) {
		 FieldPrinter.printField(Array.get(o,i),printStream);
	    }
	}
	else if (c.getName() == "java.lang.Byte") {
	    printStream.print(prefix+"<class name=\""+ c.getName()+"\""+
			      " type=\""+c.getName()+"\""
			      );
	    printStream.print(" value=" + o.toString());
	    printStream.println("/>");
	}
	else if (c.getName() == "java.lang.String") {
	    printStream.print(prefix+"<class name=\""+ c.getName()+"\""+
			      " type=\""+c.getName()+"\""
			      );
	    printStream.print(" value=" + o);
	    printStream.println("/>");
	}
	else {
	    printStream.println(prefix+"<class name="+c.getName()+">");  
	}

	try {
	    /* Inspect the fields */
	    Field f[] = c.getFields();
	    for (int i=0; i<f.length; ++i) {
		Class tf = f[i].getType();
		if (tf.isPrimitive()) {
		    printStream.print(prefix+"<field name=\""+ f[i].getName()+"\""+
				      " type=\""+tf.getName()+"\""
				      );
		    printStream.print(" value=" + f[i].get(o).toString());
		    printStream.println("/>");
		}
		else if (c.isArray()) {
		    for (i=0; i<Array.getLength(o);++i) {
			FieldPrinter.printField(Array.get(o,i),printStream);
		    }
		}
		else if (tf.getName() == "java.lang.String") {
		    printStream.print(prefix+"<field name=\""+ f[i].getName()+"\""+
				      " type=\""+tf.getName()+"\""
				      );
		    printStream.print(" value=" + f[i].get(o));
		    printStream.println("/>");
		}
		else if (tf.getName() == "java.lang.Byte") {
		    printStream.print(prefix+"<field name=\""+ f[i].getName()+"\""+
				      " type=\""+tf.getName()+"\""
				      );
		    printStream.print(" value=" + f[i].get(o).toString());
		    printStream.println("/>");
		}
		/* recurse */
		else {		    
		    FieldPrinter.printField(f[i].get(o),printStream);
		}
	    }
	}
	catch (Exception e) {
	    printStream.println(prefix+"<exception thrown/>");
	}

	/* terminate class */
	if (!c.isPrimitive()) {
	    printStream.println(prefix+"</class>");
	}
	FieldPrinter.level = FieldPrinter.level - 1;
    }
}
