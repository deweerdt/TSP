

##############################
# La base = DEVBASE
##############################
if ( ! $?DEVBASE ) then 
	setenv DEVBASE /home2/breiz/tnt/sun/tsp
endif
echo "Using DEVBASE         = $DEVBASE"


##############################
# Rajout du vxWorks
##############################
# valeur par defaut
setenv WIND_BASE /home1/breiz/PowerPC_tornado2.0/tornado.login 

# Rajout du vxWorks en T2
if ( -e /home1/breiz/68k_tornado2.0/tornado.login ) then
        source /home1/breiz/68k_tornado2.0/tornado.login
endif


if ( -e /home1/breiz/PowerPC_tornado2.0/tornado.login ) then
        source /home1/breiz/PowerPC_tornado2.0/tornado.login
endif
# login pour fic 8234 et 166
setenv VXWORKS /home1/breiz/vxworks5.2

#login pour 162
setenv VX_68K_HEADER $WIND_BASE/target/h
setenv VX_68K_TYPE $WIND_BASE/target/h/types
setenv VX_68K_GCC_LIB $WIND_BASE/host/sun4-solaris2/lib/gcc-lib/
setenv VX_68K_OPTION 

#endif

##############################
# Rajout de PATH
##############################

setenv PATH ${DEVBASE}/exec/DEV/bin.consumer:${DEVBASE}/exec/DEV/bin.provider:${PATH}:${DEVBASE}/src/scripts

##############################
# Navigation
##############################
alias go_ec 'cd ${DEVBASE}/exec/DEV/bin.consumer'
alias go_ecd 'cd ${DEVBASE}/exec/DEV/bin.consumer.debug'
alias go_ep 'cd ${DEVBASE}/exec/DEV/bin.provider'
alias go_epd 'cd ${DEVBASE}/exec/DEV/bin.provider.debug'
alias go_tsp 'cd ${DEVBASE}/src/tsp'


##############################
# CVS 
##############################
setenv CVSROOT /home2/breiz/tnt/CVSROOT
alias lcvs 'cvs -d /home2/breiz/tnt/CVSROOT '
alias gcvs 'cvs -d /home2/breiz/dev442/CVSROOT'





