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
 * 		   Christophe Pecquerie
 *
 * $Id: TspDialogAddSource.java,v 1.2 2004-02-13 12:12:01 cpecquerie Exp $
 * 
 * Changes ------- 22-Jan-2004 : Creation Date (NB);
 *  
 */

package tsp.consumer.jsynoptic.ui;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.event.ListDataEvent;
import javax.swing.event.ListDataListener;

import jsynoptic.ui.JSynoptic;
import tsp.consumer.jsynoptic.impl.TspHandler;
import tsp.consumer.jsynoptic.impl.TspSampleSymbolInfo;
import tsp.consumer.jsynoptic.impl.TspSymbolListModel;

public class TspDialogAddSource extends JDialog implements ListDataListener{
    
	private TspHandler tspHandler_;
	private TspSymbolListModel selectedListModel_;
	private TspSymbolListModel availableListModel_;
	
	private boolean[] savedSymbolSamplingTab_;
	private int bufferDuration_;
	
	private JButton addAllSymbolButton;
	private JPanel addRemovePanel_;
	private JButton addSymbolButton_;
	private JList availableList_;
	private JPanel availableSymbolsPanel_;
	private JScrollPane availableSymbolsScrollPane_;
	private JLabel bufferSizeLabel2_;
	private JLabel bufferSizeLabel3_;
	private JLabel bufferSizeLabel_;
	private JButton buttonCancel_;
	private JButton buttonOK_;
	private JPanel buttonPanel_;
	private JButton filterButton_;
	private JTextField filterInput_;
	private JLabel filterLabel_;
	private JPanel optionPanel_;
	private JPanel filterPanel_;
	private JButton removeAllSymbolButton;
	private JButton removeSymbolButton_;
	private JList selectedList_;
	private JScrollPane selectedScrollPane_;
	private JPanel selectedSymbolsPanel_;
	private JTextField timeBufferInput_;
	private JLabel timeBufferLabel_;
	
	private boolean returnOK_ = false;
	
    /** Creates new form DialogAddTspSources */
	public TspDialogAddSource(TspHandler tspHandler) {
		super(JSynoptic.gui, "TSP sampling source selection", true);
		tspHandler_ = tspHandler;
		bufferDuration_ = tspHandler_.getBufferDuration_();
		
		//Manually clone tspHandler_.symbolTab_
		savedSymbolSamplingTab_ = new boolean[tspHandler_.getSymbolTab().length];
		for (int i=0; i<savedSymbolSamplingTab_.length; i++)
			savedSymbolSamplingTab_[i] = tspHandler_.getSymbolTab()[i].sample;

		availableListModel_ = new TspSymbolListModel(tspHandler_.getSymbolTab(), true);
		selectedListModel_ = new TspSymbolListModel(tspHandler_.getSymbolTab(), false);
		
		//Init graphical components
		initComponents();
		setBufferSizeLabel();
	}
	
	public boolean showDialog() {
		super.show();
		if(!returnOK_) {
			for (int i=0; i<savedSymbolSamplingTab_.length; i++)
				tspHandler_.getSymbolTab()[i].sample = savedSymbolSamplingTab_[i];
		}
		return returnOK_;
	}
	
	public void show() {
		super.show();
		if(!returnOK_) {
			for (int i=0; i<savedSymbolSamplingTab_.length; i++)
				tspHandler_.getSymbolTab()[i].sample = savedSymbolSamplingTab_[i];
		}
	}
    
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        selectedSymbolsPanel_ = new javax.swing.JPanel();
        selectedScrollPane_ = new javax.swing.JScrollPane();
        selectedList_ = new javax.swing.JList();
        availableSymbolsPanel_ = new javax.swing.JPanel();
        availableSymbolsScrollPane_ = new javax.swing.JScrollPane();
        availableList_ = new javax.swing.JList();
        addRemovePanel_ = new javax.swing.JPanel();
        addAllSymbolButton = new javax.swing.JButton();
        addSymbolButton_ = new javax.swing.JButton();
        removeSymbolButton_ = new javax.swing.JButton();
        removeAllSymbolButton = new javax.swing.JButton();
        optionPanel_ = new javax.swing.JPanel();
        filterPanel_ = new javax.swing.JPanel();
        timeBufferLabel_ = new javax.swing.JLabel();
        timeBufferInput_ = new javax.swing.JTextField(5);
        bufferSizeLabel_ = new javax.swing.JLabel();
        bufferSizeLabel2_ = new javax.swing.JLabel();
        filterInput_ = new javax.swing.JTextField(15);
        filterLabel_ = new javax.swing.JLabel();
        bufferSizeLabel3_ = new javax.swing.JLabel();
        buttonPanel_ = new javax.swing.JPanel();
        buttonOK_ = new javax.swing.JButton();
        buttonCancel_ = new javax.swing.JButton();

        getContentPane().setLayout(new java.awt.GridBagLayout());

        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                closeDialog(evt);
            }
        });

        selectedSymbolsPanel_.setLayout(new javax.swing.BoxLayout(selectedSymbolsPanel_, javax.swing.BoxLayout.Y_AXIS));

        selectedSymbolsPanel_.setBorder(new javax.swing.border.TitledBorder("Selected symbols"));
        selectedSymbolsPanel_.setMinimumSize(new java.awt.Dimension(200, 218));
        selectedSymbolsPanel_.setPreferredSize(new java.awt.Dimension(200, 218));
        selectedScrollPane_.setPreferredSize(new java.awt.Dimension(3, 192));
        selectedList_.setBorder(new javax.swing.border.LineBorder(new java.awt.Color(0, 0, 0)));
        selectedList_.setFixedCellWidth(90);
        selectedList_.setVisibleRowCount(11);
        selectedList_.setModel(selectedListModel_);
        selectedListModel_.addListDataListener(this);
        selectedScrollPane_.setViewportView(selectedList_);

        selectedSymbolsPanel_.add(selectedScrollPane_);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 40;
        getContentPane().add(selectedSymbolsPanel_, gridBagConstraints);

        availableSymbolsPanel_.setLayout(new javax.swing.BoxLayout(availableSymbolsPanel_, javax.swing.BoxLayout.Y_AXIS));

        availableSymbolsPanel_.setBorder(new javax.swing.border.TitledBorder("Avalaible symbols"));
        availableSymbolsPanel_.setMinimumSize(new java.awt.Dimension(200, 218));
        availableSymbolsPanel_.setPreferredSize(new java.awt.Dimension(200, 218));
        availableList_.setBorder(new javax.swing.border.LineBorder(new java.awt.Color(0, 0, 0)));
        availableList_.setFixedCellWidth(90);
        availableList_.setVisibleRowCount(11);
        availableList_.setModel(availableListModel_);
        availableListModel_.addListDataListener(this);
        availableSymbolsScrollPane_.setViewportView(availableList_);

        availableSymbolsPanel_.add(availableSymbolsScrollPane_);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 40;
        getContentPane().add(availableSymbolsPanel_, gridBagConstraints);

        addRemovePanel_.setLayout(new java.awt.GridLayout(4, 1));

        addAllSymbolButton.setText(">>");
        addAllSymbolButton.setToolTipText("Add all");
        addAllSymbolButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                addAllSymbolButtonActionPerformed(evt);
            }
        });

        addRemovePanel_.add(addAllSymbolButton);

        addSymbolButton_.setText(">");
        addSymbolButton_.setToolTipText("Add selected symbols");
        addSymbolButton_.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                addSymbolButton_ActionPerformed(evt);
            }
        });

        addRemovePanel_.add(addSymbolButton_);

        removeSymbolButton_.setText("<");
        removeSymbolButton_.setToolTipText("Remove selected symbols");
        removeSymbolButton_.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                removeSymbolButton_ActionPerformed(evt);
            }
        });

        addRemovePanel_.add(removeSymbolButton_);

        removeAllSymbolButton.setText("<<");
        removeAllSymbolButton.setToolTipText("Remove all");
        removeAllSymbolButton.setMargin(new java.awt.Insets(2, 11, 2, 11));
        removeAllSymbolButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                removeAllSymbolButtonActionPerformed(evt);
            }
        });

        addRemovePanel_.add(removeAllSymbolButton);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 6;
        getContentPane().add(addRemovePanel_, gridBagConstraints);
        
        filterPanel_.setBorder(new javax.swing.border.TitledBorder("Filter symbols"));

        filterLabel_.setLabelFor(filterInput_);
        filterLabel_.setText("Filter :");
        filterPanel_.add(filterLabel_);
        
        filterInput_.setHorizontalAlignment(javax.swing.JTextField.RIGHT);
        filterInput_.setPreferredSize(new java.awt.Dimension(80, 25));
        filterInput_.addKeyListener(new KeyListener() {
        	public void keyTyped(KeyEvent arg0) {
        	}
        	public void keyPressed(KeyEvent arg0) {
        	}

        	public void keyReleased(KeyEvent arg0) {
        		String pattern = filterInput_.getText();
        		for (int i = 0; i < tspHandler_.getSymbolTab().length; i++) {
        			if((tspHandler_.getSymbolTab()[i].name.indexOf(pattern) >= 0) || (pattern.length() == 0))
        				tspHandler_.getSymbolTab()[i].display = true;
        			else
        				tspHandler_.getSymbolTab()[i].display = false;
        		}
        		contentsChanged(null);    
        	}
        });
        filterPanel_.add(filterInput_);
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        getContentPane().add(filterPanel_, gridBagConstraints);
        
        optionPanel_.setLayout(new java.awt.GridBagLayout());

        optionPanel_.setBorder(new javax.swing.border.TitledBorder("Buffer duration"));
        timeBufferLabel_.setText("Buffer duration (s) : ");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        optionPanel_.add(timeBufferLabel_, gridBagConstraints);

        timeBufferInput_.setHorizontalAlignment(javax.swing.JTextField.RIGHT);
        timeBufferInput_.setText(Integer.toString(bufferDuration_));
        timeBufferInput_.setPreferredSize(new java.awt.Dimension(50, 25));
        timeBufferInput_.addKeyListener(new KeyListener() {
			public void keyTyped(KeyEvent arg0) {
			}
			public void keyPressed(KeyEvent arg0) {
			}

			public void keyReleased(KeyEvent arg0) {
				if(timeBufferInput_.getText().length() != 0) {
					bufferDuration_ = Integer.parseInt(timeBufferInput_.getText());
				}
				setBufferSizeLabel();
			}
		});
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        optionPanel_.add(timeBufferInput_, gridBagConstraints);

        bufferSizeLabel_.setText("Size :  ");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.insets = new java.awt.Insets(2, 22, 2, 2);
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        optionPanel_.add(bufferSizeLabel_, gridBagConstraints);

        bufferSizeLabel2_.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        bufferSizeLabel2_.setPreferredSize(new Dimension(60,10));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.ipadx = 4;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        optionPanel_.add(bufferSizeLabel2_, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        getContentPane().add(optionPanel_, gridBagConstraints);

        buttonPanel_.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.RIGHT));

        buttonPanel_.setAlignmentX(1.0F);
        buttonPanel_.setPreferredSize(new java.awt.Dimension(139, 46));
        buttonOK_.setMnemonic('O');
        buttonOK_.setText("OK");
        buttonOK_.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                buttonOK_ActionPerformed(evt);
            }
        });

        buttonPanel_.add(buttonOK_);

        buttonCancel_.setMnemonic('C');
        buttonCancel_.setText("Cancel");
        buttonCancel_.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                buttonCancel_ActionPerformed(evt);
            }
        });

        buttonPanel_.add(buttonCancel_);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 8;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        getContentPane().add(buttonPanel_, gridBagConstraints);

        Dimension d=Toolkit.getDefaultToolkit().getScreenSize();				
        pack();
        setLocation((d.width-getWidth())/2,(d.height-getHeight())/2);
    }

    private void setBufferSizeLabel() {
    	int bufferSize =(int) ((selectedListModel_.nbRequested_
				* bufferDuration_
				* tspHandler_.getSamplingFrequency()
				* 8) / 1024);
    	bufferSizeLabel2_.setText(bufferSize + " KB ");
    }

    private void removeAllSymbolButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_removeAllSymbolButtonActionPerformed
    	for (int i = 0; i < tspHandler_.getSymbolTab().length; i++)
    		if (tspHandler_.getSymbolTab()[i].display)
    			tspHandler_.getSymbolTab()[i].sample = false;
    	selectedListModel_.getSize();
    }

    private void removeSymbolButton_ActionPerformed(java.awt.event.ActionEvent evt) {
        int[] indicesToRemove = selectedList_.getSelectedIndices();
    	int[] indicesToUnsample = new int[indicesToRemove.length];
    	for (int i = 0; i < indicesToRemove.length; i++) 
    		indicesToUnsample[i] = ((TspSampleSymbolInfo) selectedListModel_.getElementAt(indicesToRemove[i])).provider_global_index;
    	for (int i = 0; i < indicesToUnsample.length; i++)
    		tspHandler_.getSymbolTab()[indicesToUnsample[i]].sample = false;
    	selectedListModel_.getSize();
    	contentsChanged(null);
    }

    private void addSymbolButton_ActionPerformed(java.awt.event.ActionEvent evt) {
   		int[] indicesToAdd = availableList_.getSelectedIndices();
   		int[] indicesToSample = new int[indicesToAdd.length];
   		for (int i = 0; i < indicesToAdd.length; i++) 
   			indicesToSample[i] = ((TspSampleSymbolInfo) availableListModel_.getElementAt(indicesToAdd[i])).provider_global_index;
   		for (int i = 0; i < indicesToSample.length; i++)
   			tspHandler_.getSymbolTab()[indicesToSample[i]].sample = true;
   		//selectedListModel_.getSize();
   		contentsChanged(null);
    }

    private void addAllSymbolButtonActionPerformed(java.awt.event.ActionEvent evt) {
    	for (int i = 0; i < tspHandler_.getSymbolTab().length; i++)
    		if (tspHandler_.getSymbolTab()[i].display)
    			tspHandler_.getSymbolTab()[i].sample = true;
    	//selectedListModel_.getSize();
    	contentsChanged(null);
    }
    
    private void filterButtonActionPerformed(ActionEvent evt) {
    	String pattern = filterInput_.getText();
    	for (int i = 0; i < tspHandler_.getSymbolTab().length; i++) {
    		if((tspHandler_.getSymbolTab()[i].name.indexOf(pattern) >= 0) || (pattern.length() == 0))
    			tspHandler_.getSymbolTab()[i].display = true;
    		else
    			tspHandler_.getSymbolTab()[i].display = false;
    	}
    	contentsChanged(null);    
    }

    private void buttonCancel_ActionPerformed(java.awt.event.ActionEvent evt) {
    	setVisible(false);
    	dispose();
    }

    private void buttonOK_ActionPerformed(java.awt.event.ActionEvent evt) {
    	returnOK_ = true;
    	tspHandler_.setBufferDuration_(bufferDuration_);
    	setVisible(false);
    	dispose();
    }
    
    /** Closes the dialog */
    private void closeDialog(java.awt.event.WindowEvent evt) {
        setVisible(false);
        dispose();
    }
    
    /* (non-Javadoc)
     * @see javax.swing.event.ListDataListener#intervalAdded(javax.swing.event.ListDataEvent)
     */
    public void intervalAdded(ListDataEvent arg0) {
    	setBufferSizeLabel();
    	selectedList_.repaint();
    	availableList_.repaint();
    }

    /* (non-Javadoc)
     * @see javax.swing.event.ListDataListener#intervalRemoved(javax.swing.event.ListDataEvent)
     */
    public void intervalRemoved(ListDataEvent arg0) {
    	setBufferSizeLabel();
    	selectedList_.repaint();
    	availableList_.repaint();
    }

    /* (non-Javadoc)
     * @see javax.swing.event.ListDataListener#contentsChanged(javax.swing.event.ListDataEvent)
     */
    public void contentsChanged(ListDataEvent arg0) {
    	setBufferSizeLabel();
    	selectedList_.repaint();
    	availableList_.repaint();
    }
}
