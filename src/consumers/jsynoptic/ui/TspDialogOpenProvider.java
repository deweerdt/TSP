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
 * $Id: TspDialogOpenProvider.java,v 1.3 2004-11-22 07:05:04 sgalles Exp $
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
import javax.swing.SwingConstants;
import javax.swing.border.TitledBorder;

import jsynoptic.ui.JSynoptic;
import tsp.consumer.jsynoptic.impl.TspHandler;
import tsp.consumer.jsynoptic.impl.TspHandler.TspProviderNotFoundException;
import tsp.core.common.url.TspMalformedURLException;
import tsp.core.common.url.TspNoServerFoundException;
import tsp.core.common.url.TspURL;
import tsp.core.common.url.TspURLFactory;
import tsp.core.common.url.TspUnknownHostException;
import tsp.core.common.url.TspUnknownServerNameException;
import tsp.core.common.url.TspUnknownServerNumberException;
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
	private JTextField urlInput_;
	private JLabel urlLabel_;
	private JLabel infoChannelLabel2_;
	private JLabel infoChannelLabel_;
	private JPanel infoConnectPanel_;
	private JPanel inputConnectPanel_;
	private JPanel urlConnectPanel_;
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
		urlInput_.setText(tspHandler_.getUrl().toString());		
		
		frequencyInput_.setText(Double.toString(tspHandler_.getSamplingFrequency()));
		phaseInput_.setText(Integer.toString(tspHandler_.getSamplingPhase()));
		if(tspHandler_.getSessionId_() >= 0) {
			printStatusOK("Connected to " + tspHandler_.getUrl().getHost() + ":" + tspHandler_.getUrl().getServerNumber());
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
		inputConnectPanel_ = new JPanel();
		urlConnectPanel_ = new JPanel();
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
		urlLabel_ = new JLabel();
		urlInput_ = new JTextField();		
		frequencyLabel_ = new JLabel();
		frequencyInput_ = new JTextField();
		phaseLabel_ = new JLabel();
		phaseInput_ = new JTextField();

		getContentPane().setLayout(new GridBagLayout());

		setFont(new Font("Arial", 0, 12));
		setName("WindowOpenProvider");
		setResizable(true);
		setModal(true);
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent evt) {
				exitForm(evt);
			}
		});

		statusPanel_.setBorder(new TitledBorder("Status"));
		statusLabel_.setFont(new Font("Dialog", 1, 10));
		statusLabel_.setForeground(Color.red);
		statusPanel_.add(statusLabel_);

		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridy = 2;
		gridBagConstraints.gridwidth = 3;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		gridBagConstraints.anchor = GridBagConstraints.WEST;
		getContentPane().add(statusPanel_, gridBagConstraints);
		
		// Url Connect Panel
		urlConnectPanel_.setLayout(new GridBagLayout());
		
		urlConnectPanel_.setBorder(new TitledBorder("TSP URL"));
		urlLabel_.setFont(new Font("Dialog", 0, 10));
		urlLabel_.setLabelFor(urlInput_);
		urlInput_.setText("///");
		urlLabel_.setText("   URL format : [protocol:]//[hostname]/[servername][:servernumber]   ");
		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.gridwidth = GridBagConstraints.RELATIVE;
		urlConnectPanel_.add(urlInput_, gridBagConstraints);
		gridBagConstraints.gridwidth = GridBagConstraints.REMAINDER;
		urlConnectPanel_.add(buttonConnect_, gridBagConstraints);
		gridBagConstraints.gridwidth = GridBagConstraints.RELATIVE;
		urlConnectPanel_.add(urlLabel_,gridBagConstraints);		
		
		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridy = 0;
		gridBagConstraints.gridwidth = 3;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		gridBagConstraints.anchor = GridBagConstraints.WEST;
		getContentPane().add(urlConnectPanel_, gridBagConstraints);
		
		// Button Pannel
		buttonPanel_.setLayout(new FlowLayout(FlowLayout.RIGHT));

		//buttonPanel_.add(logoPanel_);
		
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
		gridBagConstraints.gridx = 1;
		gridBagConstraints.gridy = 3;
		gridBagConstraints.gridwidth = 2;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		getContentPane().add(buttonPanel_, gridBagConstraints);
		
		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridy = 3;
		gridBagConstraints.gridwidth = 1;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		getContentPane().add(logoPanel_, gridBagConstraints);

		infoConnectPanel_.setLayout(
			new BoxLayout(infoConnectPanel_, BoxLayout.Y_AXIS));
		
		inputConnectPanel_.setLayout(
				new BoxLayout(inputConnectPanel_, BoxLayout.Y_AXIS));

		buttonConnect_.setMnemonic('n');
		buttonConnect_.setText("Connect");
		buttonConnect_.setAlignmentY(3.0F);
		getRootPane().setDefaultButton(buttonConnect_);
		buttonConnect_.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				buttonConnect_ActionPerformed(evt);
			}
		});

		//infoPanel
		infoPanel_.setLayout(new GridLayout(5, 2));

		infoPanel_.setBorder(new TitledBorder("Infos"));
		infoChannelLabel_.setFont(new Font("Dialog", 0, 12));
		infoChannelLabel_.setText("Channel ID :");
		infoChannelLabel_.setHorizontalAlignment(SwingConstants.RIGHT);
		infoPanel_.add(infoChannelLabel_);

		infoChannelLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoChannelLabel2_);

		infoVersionLabel_.setFont(new Font("Dialog", 0, 12));
		infoVersionLabel_.setText("Version :");
		infoVersionLabel_.setHorizontalAlignment(SwingConstants.RIGHT);
		infoPanel_.add(infoVersionLabel_);

		infoVersionLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoVersionLabel2_);

		infoMaxFreqLabel_.setFont(new Font("Dialog", 0, 12));
		infoMaxFreqLabel_.setText("Max Freq :");
		infoMaxFreqLabel_.setHorizontalAlignment(SwingConstants.RIGHT);
		infoPanel_.add(infoMaxFreqLabel_);

		infoMaxFreqLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoMaxFreqLabel2_);

		infoNbClientsLabel_.setFont(new Font("Dialog", 0, 12));
		infoNbClientsLabel_.setText("Nb clients :");
		infoNbClientsLabel_.setHorizontalAlignment(SwingConstants.RIGHT);
		infoPanel_.add(infoNbClientsLabel_);

		infoNbClientsLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoNbClientsLabel2_);

		infoNbSymbolsLabel_.setFont(new Font("Dialog", 0, 12));
		infoNbSymbolsLabel_.setText("Nb symbols : ");
		infoNbSymbolsLabel_.setHorizontalAlignment(SwingConstants.RIGHT);
		infoPanel_.add(infoNbSymbolsLabel_);

		infoNbSymbolsLabel2_.setFont(new Font("Dialog", 1, 12));
		infoPanel_.add(infoNbSymbolsLabel2_);

		infoConnectPanel_.add(infoPanel_);

		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 2;
		gridBagConstraints.gridy = 1;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		getContentPane().add(infoConnectPanel_, gridBagConstraints);

		// inputConnectPanel		
		
		inputPanel_.setLayout(new GridLayout(3, 2, 0, 5));
				
		inputPanel_.setBorder(new TitledBorder("TSP Config"));
		/*urlLabel_.setFont(new Font("Dialog", 0, 12));
		urlLabel_.setLabelFor(urlInput_);
		urlLabel_.setText("Url : ");		
		inputPanel_.add(urlLabel_);		
		inputPanel_.add(urlInput_);*/
		
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

		inputConnectPanel_.add(inputPanel_);
		
		
		gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridy = 1;
		gridBagConstraints.gridwidth = 2;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.ipady = 6;
		gridBagConstraints.insets = new Insets(2, 2, 2, 2);
		getContentPane().add(inputConnectPanel_, gridBagConstraints);
		
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
		
	/*	if (hostName.equals(""))
			printStatusError("Please enter a hostname");
		else {*/
			try {
				TspURL url = TspURLFactory.createWithDefaultSupport(urlInput_.getText());				
				tspHandler_ = new TspHandler(url);
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
				printStatusOK("Connected to " + url );
				buttonOK_.setEnabled(true);
			} catch (TspMalformedURLException e) {
				clearInfoLabels();
				printStatusError("Malformed URL");
				buttonOK_.setEnabled(false);
				tspHandler_ = null;
			} catch (TspUnknownHostException e) {				
				clearInfoLabels();
				printStatusError("Unknown Host");
				buttonOK_.setEnabled(false);
				tspHandler_ = null;
			} catch (TspUnknownServerNameException e) {				
				clearInfoLabels();
				printStatusError("Unknown ServerName");
				buttonOK_.setEnabled(false);
				tspHandler_ = null;
			} catch (TspUnknownServerNumberException e) {				
				clearInfoLabels();
				printStatusError("Unknown ServerNumber");
				buttonOK_.setEnabled(false);
				tspHandler_ = null;
			} catch (TspNoServerFoundException e) {				
				clearInfoLabels();
				printStatusError("No server found on host");
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
