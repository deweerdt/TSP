/*

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer_main.c,v 1.16 2006-10-18 21:22:34 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : Consumer

-----------------------------------------------------------------------

Purpose   : TSP ascii writer consumer (main)

-----------------------------------------------------------------------
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <tsp_ascii_writer.h>
#include <tsp_consumer.h>
#include <tspcfg_file.h>

/**
 * @defgroup TSP_AsciiWriter ASCII Writer
 * A TSP ascii writer consumer.
 * a TSP consumer which is able to output symbols values in different ASCII file format. 
 * It's output may be standard output or file with other options to chose file format and eventual 
 * size limit. It's main purposes is to be able to export TSP distributed symbols and value to 
 * some kind of CSV (Comma Separated Value) format in order to be easily post processed by 
 * spreadsheet softwares or simpler plotting software like 
 * <a href="http://www.gnuplot.org/">Gnuplot (http://www.gnuplot.org/)</a>.
 * You may specify different file format output which essentialy change the header of the file. 
 *
 * \par tsp_ascii_writer [-n] -x=\<sample_config_file\> [-o=\<output_filename\>] [-f=\<output file format\>] [-l=\<nb sample\>] [-u TSP_provider URL ] 
 * \par 
 * <ul>
 *   <li> \b -n  (optional) will check and enforce no duplicate symbols</li>
 *   <li> \b -x  the file specifying the list of symbols to be sampled</li>
 *   <li> \b -f  (optional) specifying the format of output file. Recognized file format are
 *               <ul>
 *                 <li> \b simple_ascii  tabulated ascii no header</li>
 *                 <li> \b bach          tabulated ascii with BACH header</li>
 *                 <li> \b macsim        tabulated ascii with MACSIM header</li>
 *               </ul>
 *               Default is \b simple_ascii.
 *   </li>
 *   <li> \b -o  (optional) the name of the output file. If not specified standard out is used.</li>
 *   <li> \b -l  (optional) the maximum number of sample to be stored in file. Unlimited if not specified.</li>
 *   <li> \b -u  (optional) the TSP provider URL. If not specified default is localhost.</li>
 * </ul>
 * @ingroup TSP_Consumers
 */


static void 
my_sighandler(int signum) {
  fprintf(stderr,"tsp_ascii_writer::Captured signal<%d>\n",signum);
  fflush(stderr);
  tsp_ascii_writer_stop();    
}
                                                                                                 
int
main (int argc, char* argv[]) {
  
  struct sigaction my_action;
  struct sigaction old_action;
  int32_t                             retcode=0;
  uint32_t                            nbUniqueSymbols;
  TSP_sample_symbol_info_list_t       validatedSymbolList;
  TSP_sample_symbol_info_list_t       configSymbolList;

  TSP_sample_symbol_info_list_t       *configXMLSymbolList;

  char*   input_filename  = NULL;
  char*   output_filename = NULL;
  char*   file_format     = NULL;
  FILE*   output_stream   = NULL;
  int32_t output_limit    = 0;
  char*   provider_url    = "rpc://localhost/";
  int32_t no_duplicate    = 0;

  int32_t  continu=1;
  int32_t  indice_provider=0;
  int32_t  is_xml_config  = 0;
  TspCfg_T xmlconfig;
  TspCfgProviderList_T* provider_list=NULL;


  /* set up default output file format */
  int header_style  		  = SimpleAsciiTabulated_FileFmt;

  /* Main options handling */
  char*         errorString;
  int           opt_ok=1;
  char          c_opt;
    
  if (argc < 2) {
    opt_ok  = 0;
    retcode = -1;
    fprintf(stderr,"%s: Insufficient number of options\n",argv[0]);
  }

  /* Analyse command line parameters */
  c_opt = getopt(argc,argv,"x:u:o:l:f:hn");

  if(opt_ok && EOF != c_opt)
  {
    opt_ok  = 1;
    do
    {    
      switch (c_opt) {
      case 'x':
	input_filename = strdup(optarg);
	fprintf(stdout,"%s: sample config file is <%s>\n",argv[0],input_filename);
	/* poor auto-detect file format
	 * FIXME: should check first line of file for xml encoding line 
	 */
	if ((NULL != strstr(input_filename,".xml")) || 
	    (NULL != strstr(input_filename,".XML"))
	    ) {
	  is_xml_config=1;
	  fprintf(stdout,"%s: XML config file format detected\n",argv[0]);
	}
	break;
      case 'o':
	output_filename = strdup(optarg);
	fprintf(stdout,"%s: output sample file is <%s>\n",argv[0],output_filename);
	break;
      case 'f':
	file_format = strdup(optarg);
	fprintf(stdout,"%s: provided output file format is <%s>\n",argv[0],file_format);
	OutputFileFormat_t fmt;
	for (fmt=SimpleAsciiTabulated_FileFmt;fmt<LAST_FileFmt;++fmt) {
	  if (strcasecmp(file_format,OutputFileFormat_name_tab[fmt])==0) {
	    header_style=fmt;
	    break;
	  }
	}
	break;  
      case 'l':
	errorString = NULL;
	output_limit = strtol(optarg,&errorString,10);
	if ('\0' != *errorString) {
	  fprintf(stderr,"%s: incorrect output limit format : <%s> (error begin at <%s>)\n",argv[0],optarg, errorString);
	  opt_ok = 0;
	} else {
	  fprintf(stderr,"%s: TSP sample output file limited to <%d> sample(s).\n",argv[0],output_limit);
	}
	break;
      case 'u':
	provider_url = strdup(optarg);
	fprintf(stdout,"%s: TSP provider URL is <%s>\n",argv[0],provider_url);
	break;
      case 'n':
	no_duplicate = 1;
	fprintf(stdout,"%s: will enforce no duplicate symbol\n",argv[0]);
	break;
      case '?':
	fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
	opt_ok = 0;
	break;
      default:
	opt_ok = 0;
	break;
      } /* end of switch */  
      c_opt = getopt(argc,argv,"x:u:o:l:f:hn");  
    }
    while (opt_ok && (EOF != c_opt));
  }
  else
  {
    opt_ok = 0;
  }


  if (!opt_ok) {
    printf("Usage: %s [-n] -x=<sample_config_file> [-o=<output_filename>] [-f=<output file format] [-l=<nb sample>] [-u=<TSP_URL>]\n",
	   argv[0]);
    printf("   -n   will check and enforce no duplicate symbols\n");
    printf("   -x   the file specifying the list of symbols to be sampled (.dat or .XML)\n");
    printf("   -f   (optional) specifying the format of output file\n");
    printf("        possible formats are:\n");
    printf("           simple_ascii : tabulated ascii no header \n");
    printf("           bach         : tabulated ascii with BACH header\n");
    printf("           macsim       : tabulated ascii with MACSIM header\n");
    printf("   -o   (optional) the name of the output file\n");
    printf("   -l   (optional) the maximum number of sample to be stored in file\n");
    printf("   -u   (optional) the  TSP provider URL <TSP_URL> \n");
    printf("%s",TSP_URL_FORMAT_USAGE);
    exit(retcode);
  }
  
  fprintf(stdout,"%s: selected output file format is <%s>\n",argv[0],OutputFileFormat_desc_tab[header_style]);
  retcode = tsp_ascii_writer_initialise(&argc,&argv);

  if (TSP_STATUS_OK!=retcode) {
    fprintf(stderr,"%s: Initialise failed: <%d>\n",argv[0],retcode);
    exit(-1);
  }
    
  /* install SIGINT handler (POSIX way) */
  my_action.sa_handler = &my_sighandler;  
  sigfillset(&my_action.sa_mask);
  my_action.sa_flags = SA_RESTART;
  sigaction(SIGINT,&my_action,&old_action);    

  /* reset config list */

  fprintf(stdout,"%s: Load config file...\n",argv[0]);

  TSP_SSIList_initialize(&configSymbolList,1);
  TSP_SSIList_finalize(&configSymbolList);

  if(0==is_xml_config)
  {
   
    retcode = tsp_ascii_writer_load_config(input_filename,
					   &(configSymbolList.TSP_sample_symbol_info_list_t_val),
					   &(configSymbolList.TSP_sample_symbol_info_list_t_len));
  }
  else
  {
    retcode=TSP_TspCfg_load(&xmlconfig,input_filename);

  }


  if (TSP_STATUS_OK!=retcode) {
    fprintf(stderr,"<%s>: Invalid configuration file (%d parse error(s)).\n",
	    input_filename, tsp_ascii_writer_parse_error);
    tsp_ascii_writer_stop();
  }


  /* we use an XML config file, so we must search a provider which can be reach by your computer,
   * so we search the fisrt provider tha can be reach
   */
  if(1==is_xml_config)
  {
    /* retrieve the provider list contain in the xml file*/
    provider_list=TSP_TspCfg_getProviderList(&xmlconfig);
    if (NULL==provider_list) {
      fprintf(stderr,"There are no provider in the XML file.\n");
      fflush(stdout);      
      tsp_ascii_writer_stop();
      retcode=TSP_STATUS_NOK;
      
    }
    else
    {
      /*for each provider we look that he could be reach*/
      continu=1;
      for(indice_provider=0;indice_provider<provider_list->length && continu;++indice_provider)
      {
	retcode=TSP_STATUS_OK;

	/*retrieve the sample list of the provider*/
	configXMLSymbolList=TSP_TspCfg_getProviderSampleList(&xmlconfig,provider_list->providers[indice_provider].name);

	TSP_SSIList_copy(&configSymbolList,*configXMLSymbolList);

	nbUniqueSymbols = TSP_SSIList_getSize(configSymbolList);
    
	if(0!=nbUniqueSymbols)
	{
	  if (TSP_STATUS_OK==retcode && no_duplicate) {
	    retcode = tsp_ascii_writer_make_unique(&(configSymbolList.TSP_sample_symbol_info_list_t_val),
						   &(nbUniqueSymbols));

	    if (TSP_STATUS_OK!=retcode) {
	      fprintf(stderr,
		      "Configuration file contains duplicate symbols with different period,\n");
	      fprintf(stderr,"  --> Please fix your config file <%s> and re-run.\n",input_filename);
	      fprintf(stderr,"  --> It seems to be symbol <%s>\n",
		      TSP_SSIList_getSSI(configSymbolList,nbUniqueSymbols)->name);
	      tsp_ascii_writer_stop();
	      continu=0;
	    } else {
	      configSymbolList.TSP_sample_symbol_info_list_t_len = nbUniqueSymbols;
	    }
	  }
	  
	  if (TSP_STATUS_OK==retcode) {
	    fprintf(stdout,"%s: Validate symbols against provider info...\n",argv[0]);
	    fflush(stdout);
	    
	    retcode = tsp_ascii_writer_validate_symbols(&configSymbolList,provider_list->providers[indice_provider].url,
							&validatedSymbolList);
	    if(TSP_STATUS_OK!=retcode) {
	      fprintf(stdout,"The provider %s: can be reach, try with the next...\n",provider_list->providers[indice_provider].name);
	      fflush(stdout);
	  }
	    else
	      {
		/*the provider can be reach*/
		continu=0;
	      }
	  }
	}
      }
    }       
  }
  else
  {
      nbUniqueSymbols = TSP_SSIList_getSize(configSymbolList);

      if (TSP_STATUS_OK==retcode && no_duplicate) {
	retcode = tsp_ascii_writer_make_unique(&(configSymbolList.TSP_sample_symbol_info_list_t_val),
					       &(nbUniqueSymbols));

	if (TSP_STATUS_OK!=retcode) {
	  fprintf(stderr,
		  "Configuration file contains duplicate symbols with different period,\n");
	  fprintf(stderr,"  --> Please fix your config file <%s> and re-run.\n",input_filename);
	  fprintf(stderr,"  --> It seems to be symbol <%s>\n",
		  TSP_SSIList_getSSI(configSymbolList,nbUniqueSymbols)->name);
	  tsp_ascii_writer_stop();
	} else {
	  configSymbolList.TSP_sample_symbol_info_list_t_len = nbUniqueSymbols;
	}
      }


      if (TSP_STATUS_OK==retcode) {
	fprintf(stdout,"%s: Validate symbols against provider info...\n",argv[0]);
	fflush(stdout);
	retcode = tsp_ascii_writer_validate_symbols(&configSymbolList,provider_url,
						&validatedSymbolList);
      }

  }

  if (TSP_STATUS_OK==retcode) {
    fprintf(stdout,"%s: Ascii writer running...\n",argv[0]);
    fflush(stdout);  
    if (NULL == output_filename) {
      output_stream = stdout;
      output_limit  = 0;
    } else {
      output_stream = fopen(output_filename,"w");
      if ((FILE*)NULL == output_stream) {
	fprintf(stderr,"Cannot open output file <%s> for writing\n",output_filename);
	retcode = TSP_STATUS_ERROR_AW_OUTPUT_FILE_ERROR;
      }    
    }
  }
  
  if (TSP_STATUS_OK == retcode) {
    retcode = tsp_ascii_writer_start(output_stream,output_limit,header_style,&validatedSymbolList);
  }



  fprintf(stdout,"%s: Ascii writer stopped...\n",argv[0]);
  fflush(stdout);  
  tsp_ascii_writer_finalise();

  return (retcode);
} /* end of main */
