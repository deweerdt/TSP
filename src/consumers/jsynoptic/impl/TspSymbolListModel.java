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
 * $Id: TspSymbolListModel.java,v 1.1 2004-02-02 10:52:01 dufy Exp $
 * 
 * Changes ------- 20-Jan-2004 : Creation Date (NB);
 *  
 */
package tsp.consumer.jsynoptic.impl;

import javax.swing.AbstractListModel;

/**
 * @author pecquerie
 *
 * To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Generation - Code and Comments
 */
public class TspSymbolListModel extends AbstractListModel {
	
	private TspSampleSymbolInfo[] symbolTab_;
	private boolean isLeftList_;
	public int nbAvailable_;
	public int nbRequested_;
	
	public TspSymbolListModel(TspSampleSymbolInfo[] symbolTab, boolean isAvailableList) {
		super();
		symbolTab_ = symbolTab;
		isLeftList_ = isAvailableList;
	}

	/* (non-Javadoc)
	 * @see javax.swing.ListModel#getSize()
	 */
	public int getSize() {
		int nb = 0;
		if (isLeftList_) {
			for(int i = 0; i < symbolTab_.length; i++ )
				if(symbolTab_[i].display && !symbolTab_[i].sample)
					nb++;
			if(nbAvailable_ != nb) {
				nbAvailable_ = nb;
				fireContentsChanged(this, 0, nb-1);
			}
		}
		else {
			for(int i = 0; i < symbolTab_.length; i++ )
				if(symbolTab_[i].display && symbolTab_[i].sample)
					nb++;
			if(nbRequested_ != nb) {
				nbRequested_ = nb;
				fireContentsChanged(this, 0, nb-1);
			}
		}
		return nb;
	}

	/* (non-Javadoc)
	 * @see javax.swing.ListModel#getElementAt(int)
	 */
	public Object getElementAt(int index) {
		int i = 0, nbDisplay = 0;
		if (isLeftList_) {
			while((nbDisplay <= index) && (i < symbolTab_.length)) {
				if(symbolTab_[i].display && !symbolTab_[i].sample)
					nbDisplay++;
				i++;
			}
		}
		else {
			while((nbDisplay <= index) && (i < symbolTab_.length)) {
				if(symbolTab_[i].display && symbolTab_[i].sample)
					nbDisplay++;
				i++;
			}			
		}
		if (nbDisplay == index + 1)
			return symbolTab_[i-1];
		else
			return null;
	}
}
