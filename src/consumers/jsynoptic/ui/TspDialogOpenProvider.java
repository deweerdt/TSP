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
 * $Id: TspDialogOpenProvider.java,v 1.2 2004-02-13 12:12:01 cpecquerie Exp $
 * 
 * Changes ------- 14-Jan-2004 : Creation Date (NB);
 *  
 */
package tsp.consumer.jsynoptic.ui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.Insets;
import java.awt.MediaTracker;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.net.UnknownHostException;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.TitledBorder;

import jsynoptic.ui.JSynoptic;
import tsp.consumer.jsynoptic.impl.TspHandler;
import tsp.consumer.jsynoptic.impl.TspHandler.TspProviderNotFoundException;
/**
 * @author pecquerie
 * 
 * To change the template for this generated type comment go to Window -
 * Preferences - Java - Code Generation - Code and Comments
 */
public class TspDialogOpenProvider extends JDialog {

	private TspHandler tspHandler_;
	private double tempSamplingFrequency_;

	private JButton buttonCancel_;
	private JButton buttonConnect_;
	private JButton buttonOK_;
	private JPanel buttonPanel_;
	private JTextField frequencyInput_;
	private JLabel frequencyLabel_;
	private JTextField hostInput_;
	private JLabel hostLabel_;
	private JLabel infoChannelLabel2_;
	private JLabel infoChannelLabel_;
	private JPanel infoConnectPanel_;
	private JLabel infoMaxFreqLabel2_;
	private JLabel infoMaxFreqLabel_;
	private JLabel infoNbClientsLabel2_;
	private JLabel infoNbClientsLabel_;
	private JLabel infoNbSymbolsLabel2_;
	private JLabel infoNbSymbolsLabel_;
	private JPanel infoPanel_;
	private JLabel infoVersionLabel2_;
	private JLabel infoVersionLabel_;
	private JPanel inputPanel_;
	private JTextField phaseInput_;
	private JLabel phaseLabel_;
	private JTextField providerInput_;
	private JLabel providerLabel_;
	private JLabel statusLabel_;
	private JPanel statusPanel_;
	private TspLogoPanel logoPanel_;

	
	public static void main(String[] args) {
		new TspDialogOpenProvider();
		System.exit(0);
	}

	public TspDialogOpenProvider() {
		super(JSynoptic.gui, "Open a TSP provider", true);
		initComponents();
		show();
	}
	
	public TspDialogOpenProvider(TspHandler tspHandler) {
		super(JSynoptic.gui, "Open a TSP provider", true);
		tspHandler_ = tspHandler;
		initComponents();
		hostInput_.setText(tspHandler_.getHostname());
		providerInput_.setText(Integer.toString(tspHandler_.getProviderId()));
		frequencyInput_.setText(Double.toString(tspHandler_.getSamplingFrequency()));
		phaseInput_.setText(Integer.toString(tspHandler_.getSamplingPhase()));
		if(tspHandler_.getSessionId_() >= 0) {
			printStatusOK("Connected to " + tspHandler_.getHostname() + ":" + tspHandler_.getProviderId());
			buttonOK_.setEnabled(true);
			infoChannelLabel2_.setText(
					Integer.toString(tspHandler_.getProviderChannelId()));
			infoNbClientsLabel2_.setText(
					Integer.toString(tspHandler_.getProviderNbClients()));
			infoVersionLabel2_.setText(
					Integer.toString(tspHandler_.getProviderVersion()));
			infoNbSymbolsLabel2_.setText(
					Integer.toString(tspHandler_.getProviderNbSymbols()));
			infoMaxFreqLabel2_.setText(
					Double.toString(tspHandler_.getProviderBaseFrequency()));
			getRootPane().setDefaultButton(buttonOK_);
		}
		else
			printStatusError("No TSP provider found on this host");
		
		show();
	}

	private void initComponents() {
		GridBagConstraints gridBagConstraints;

		statusPanel_ = new JPanel();
		statusLabel_ = new JLabel(" ");
		buttonPanel_ = new JPanel();
		buttonOK_ = new JButton();
		buttonCancel_ = new JButton();
		logoPanel_ = new TspLogoPanel();
		infoConnectPanel_ = new JPanel();
		buttonConnect_ = new JButton();
		infoPanel_ = new JPanel();
		infoChannelLabel_ = new JLabel();
		infoChannelLabel2_ = new JLabel();
		infoVersionLabel_ = new JLabel();
		infoVersionLabel2_ = new JLabel();
		infoMaxFreqLabel_ = new JLabel();
		infoMaxFreqLabel2_ = new JLabel();
		infoNbClientsLabel_ = new JLabel();
		infoNbClientsLabel2_ = new JLabel();
		infoNbSymbolsLabel_ = new JLabel();
		infoNbSymbolsLabel2_ = new JLabel();
		inputPanel_ = new JPanel();
		hostLabel_ = new JLabel();
		hostInput_ = new JTextField();
		providerLabel_ = new JLabel();
		providerInput_ = new JTextField();
		frequencyLabel_ = new JLabel();
		frequencyInput_ = new JTextField();
		phaseLabel_ = new JLabel();
		phaseInput_ = new JTextField();

		getContentPane().setLayout(new GridBagLayout());

		setFont(new Font("Arial", 0, 12));
		setName("WindowOpenProvider");
		setResizable(false);
		setModal(true);
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent evt) {
				exitForm(evt);
			}
		});

		statusPanel_.setBorder(new TitledBorder("Status"));
		statusLabel_.setFont(new Font("Dialog", 1, 12));
		statusLabel_.setForeground(Color.red);
		statusPanel_.add(statusLabel_);

		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridy = 1;
		gridBagConstraints.gridwidth = 3;
		gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		gridBagConstraints.anchor = GridBagConstraints.WEST;
		getContentPane().add(statusPanel_, gridBagConstraints);

		buttonPanel_.setLayout(new FlowLayout(FlowLayout.RIGHT));

		buttonPanel_.add(logoPanel_);
		
		buttonOK_.setMnemonic('O');
		buttonOK_.setText("OK");
		buttonOK_.setEnabled(false);
		buttonOK_.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				buttonOK_ActionPerformed(evt);
			}
		});

		buttonPanel_.add(buttonOK_);

		buttonCancel_.setMnemonic('C');
		buttonCancel_.setText("Cancel");
		buttonCancel_.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				buttonCancel_ActionPerformed(evt);
			}
		});

		buttonPanel_.add(buttonCancel_);

		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridy = 2;
		gridBagConstraints.gridwidth = 3;
		gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		getContentPane().add(buttonPanel_, gridBagConstraints);

		infoConnectPanel_.setLayout(
			new BoxLayout(infoConnectPanel_, BoxLayout.Y_AXIS));

		buttonConnect_.setMnemonic('n');
		buttonConnect_.setText("Connect");
		buttonConnect_.setAlignmentY(3.0F);
		getRootPane().setDefaultButton(buttonConnect_);
		buttonConnect_.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				buttonConnect_ActionPerformed(evt);
			}
		});

		infoConnectPanel_.add(buttonConnect_);

		infoPanel_.setLayout(new GridLayout(5, 2));

		infoPanel_.setBorder(new TitledBorder("Infos"));
		infoChannelLabel_.setFont(new Font("Dialog", 0, 12));
		infoChannelLabel_.setText("Channel ID :");
		infoPanel_.add(infoChannelLabel_);

		infoChannelLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoChannelLabel2_);

		infoVersionLabel_.setFont(new Font("Dialog", 0, 12));
		infoVersionLabel_.setText("Version :");
		infoPanel_.add(infoVersionLabel_);

		infoVersionLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoVersionLabel2_);

		infoMaxFreqLabel_.setFont(new Font("Dialog", 0, 12));
		infoMaxFreqLabel_.setText("Max Freq :");
		infoPanel_.add(infoMaxFreqLabel_);

		infoMaxFreqLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoMaxFreqLabel2_);

		infoNbClientsLabel_.setFont(new Font("Dialog", 0, 12));
		infoNbClientsLabel_.setText("Nb clients :");
		infoPanel_.add(infoNbClientsLabel_);

		infoNbClientsLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoNbClientsLabel2_);

		infoNbSymbolsLabel_.setFont(new Font("Dialog", 0, 12));
		infoNbSymbolsLabel_.setText("Nb symbols : ");
		infoPanel_.add(infoNbSymbolsLabel_);

		infoNbSymbolsLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoNbSymbolsLabel2_);

		infoConnectPanel_.add(infoPanel_);

		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 2;
		gridBagConstraints.gridy = 0;
		gridBagConstraints.fill = GridBagConstraints.VERTICAL;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		getContentPane().add(infoConnectPanel_, gridBagConstraints);

		inputPanel_.setLayout(new GridLayout(4, 2, 0, 5));

		inputPanel_.setBorder(new TitledBorder("TSP Config"));
		hostLabel_.setFont(new Font("Dialog", 0, 12));
		hostLabel_.setLabelFor(hostInput_);
		hostLabel_.setText("Host : ");
		inputPanel_.add(hostLabel_);

		inputPanel_.add(hostInput_);

		providerLabel_.setFont(new Font("Dialog", 0, 12));
		providerLabel_.setLabelFor(providerInput_);
		providerLabel_.setText("Provider : ");
		inputPanel_.add(providerLabel_);

		providerInput_.setText("0");
		inputPanel_.add(providerInput_);

		frequencyLabel_.setFont(new Font("Dialog", 0, 12));
		frequencyLabel_.setLabelFor(frequencyInput_);
		frequencyLabel_.setText("Freq (Hz) : ");
		inputPanel_.add(frequencyLabel_);

		frequencyInput_.setText("1");
		inputPanel_.add(frequencyInput_);

		phaseLabel_.setFont(new Font("Dialog", 0, 12));
		phaseLabel_.setLabelFor(phaseInput_);
		phaseLabel_.setText("Phase :");
		inputPanel_.add(phaseLabel_);

		phaseInput_.setText("0");
		inputPanel_.add(phaseInput_);

		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridy = 0;
		gridBagConstraints.gridwidth = 2;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.ipady = 6;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		getContentPane().add(inputPanel_, gridBagConstraints);
		
		Dimension d=Toolkit.getDefaultToolkit().getScreenSize();				
		pack();
		setLocation((d.width-getWidth())/2,(d.height-getHeight())/2);
	}

	private void exitForm(WindowEvent evt) {
		setVisible(false);
		if (tspHandler_ != null)
			tspHandler_.finalize();
		dispose();
	}

	private void buttonConnect_ActionPerformed(ActionEvent evt) {
		if (tspHandler_ != null)
			tspHandler_.finalize();
		String hostName = hostInput_.getText();
		int provider = Integer.parseInt(providerInput_.getText());
		if (hostName.equals(""))
			printStatusError("Please enter a hostname");
		else {
			try {
				tspHandler_ = new TspHandler(hostName, provider);
				infoChannelLabel2_.setText(
					Integer.toString(tspHandler_.getProviderChannelId()));
				infoNbClientsLabel2_.setText(
					Integer.toString(tspHandler_.getProviderNbClients()));
				infoVersionLabel2_.setText(
					Integer.toString(tspHandler_.getProviderVersion()));
				infoNbSymbolsLabel2_.setText(
					Integer.toString(tspHandler_.getProviderNbSymbols()));
				infoMaxFreqLabel2_.setText(
					Double.toString(tspHandler_.getProviderBaseFrequency()));
				printStatusOK("Connected to " + hostName + ":" + provider);
				buttonOK_.setEnabled(true);
			} catch (UnknownHostException e) {
				clearInfoLabels();
				printStatusError("Unknown Host");
				buttonOK_.setEnabled(false);
				tspHandler_ = null;
			} catch (TspProviderNotFoundException e) {
				clearInfoLabels();
				printStatusError("No TSP provider found on this host");
				buttonOK_.setEnabled(false);
				tspHandler_ = null;
			} catch (Exception e) {
				clearInfoLabels();
				printStatusError(e.getMessage());
				buttonOK_.setEnabled(false);
				tspHandler_ = null;
			}
		}
	}

	private void clearInfoLabels() {
		infoChannelLabel2_.setText("");
		infoMaxFreqLabel2_.setText("");
		infoNbClientsLabel2_.setText("");
		infoNbSymbolsLabel2_.setText("");
		infoVersionLabel2_.setText("");
	}

	private void printStatusError(String msg) {
		statusLabel_.setForeground(Color.red);
		statusLabel_.setText(msg);
	}

	private void printStatusOK(String msg) {
		statusLabel_.setForeground(Color.green);
		statusLabel_.setText(msg);
	}
	
	private boolean isEntier(double number) {
		if(number % (int) number != 0.0)
			return false;
		else
			return true;
	}

	private void buttonOK_ActionPerformed(ActionEvent evt) {
		double samplingFrequency = Double.parseDouble(frequencyInput_.getText());
		int samplingPhase = Integer.parseInt(phaseInput_.getText());
		double tempPeriod = tspHandler_.getProviderBaseFrequency()/samplingFrequency;
		
		if(tempPeriod < 1.0) {
			printStatusError("Frequency over provider limits : reset to max");
			frequencyInput_.setText(Double.toString(tspHandler_.getProviderBaseFrequency()));
			return;
		}
		
		if(tempPeriod > tspHandler_.getProviderMaxPeriod()) {
			printStatusError("Frequency under provider limits : reset to min");
			frequencyInput_.setText(Double.toString(tspHandler_.getProviderBaseFrequency()/tspHandler_.getProviderMaxPeriod()));
			return;
		}
		
		if(!isEntier(tempPeriod) && samplingFrequency != tempSamplingFrequency_){
			
			double newFrequency = (tspHandler_.getProviderBaseFrequency()/(int) Math.round(tempPeriod));
			String newFrequencyString = Double.toString(newFrequency);
			int dotIndex = newFrequencyString.indexOf('.');
			newFrequencyString = newFrequencyString.substring(0,dotIndex + 2);
			tempSamplingFrequency_ = Double.parseDouble(newFrequencyString);
			frequencyInput_.setText(newFrequencyString);
			printStatusError("Frequency has been rounded to " + newFrequencyString);
			return;
		}
		
		tspHandler_.setSamplingPhase(samplingPhase);
		tspHandler_.setPeriod_((int) Math.round(tempPeriod));
		setVisible(false);
		dispose();
		
	}

	private void buttonCancel_ActionPerformed(ActionEvent evt) {
		setVisible(false);
		if (tspHandler_ != null) {
			tspHandler_.finalize();
			tspHandler_ = null;
		}
		dispose();
	}
	
	public TspHandler getTspHandler() {
		return tspHandler_;
	}
	
	private class TspLogoPanel extends JPanel {
		private Image logo_;
		private MediaTracker mediaTracker_;
		
		/**
		 * Constructor
		 * Using a Mediatracker to wait for the images
		 * to be loaded properly
		 *
		 */
		public TspLogoPanel() {
			super();
			mediaTracker_ = new MediaTracker(this);
			//The logo
			logo_ = Toolkit.getDefaultToolkit().getImage(TspLogoPanel.class.getResource("ressources/tsp_pieuvre3.png"));
			mediaTracker_.addImage(logo_, 1);
			try {
				mediaTracker_.waitForAll();
			}
			catch(InterruptedException e) {}
		}

		/**
		 * This method overloads the JPanel paintComponent to
		 * draw the images
		 */
		public void paintComponent(Graphics g) {
			super.paintComponent(g);
			g.drawImage(logo_, 0, 0, this);
		}
		
		/**
		 * Gets the preffered size of the clock panel
		 */	
		public Dimension getPreferredSize() {
			return new Dimension (logo_.getWidth(this) + 5, logo_.getHeight(this)+5);
		}
		
		/**
		 * Gets the maximum size of the clock panel
		 */
		public Dimension getMaximumSize() {
			return new Dimension (logo_.getWidth(this) + 5, logo_.getHeight(this)+5);
		}
		
		/**
		 * Gets the minimum size of the clock panel
		 */
		public Dimension getMinimumSize() {
			return new Dimension (logo_.getWidth(this) + 5, logo_.getHeight(this)+5);
		}
	}
	
}
