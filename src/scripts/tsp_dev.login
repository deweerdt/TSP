#!/bin/tcsh
##############################
# Introspection
##############################

set HOST_UNAME=`uname -a | awk '{ print $1 }'`

#Linux
if ( "$HOST_UNAME" == "Linux" )  then
	setenv HOST_TARGET "linux"

#Sun & Sun64
else if ( "$HOST_UNAME" == "SunOS") then
    #try to find isalist util
    set SPARC_TYPE=` isalist | awk '{ print $1 }' | grep sparcv9`
    if( $? == 0 ) then
	if( $SPARC_TYPE != "" ) then
	    setenv HOST_TARGET "sun64"
	else
	    setenv HOST_TARGET "sun"
	endif	
    else
	setenv HOST_TARGET "sun"
    endif

#dec
else if ( "$HOST_UNAME" == "OSF1") then
      setenv HOST_TARGET "dec"

#???
else
     echo "ERROR : Unknown system : $HOST_UNAME"
     exit -1
endif


#Manage multiple mount points
if ( "$HOST_TARGET" != "dec" ) then
	set LOCAL_MOUNT="/home2/breiz"
else
	set LOCAL_MOUNT="/net/breiz/home2/breiz"
endif

setenv HOST_BASE "$LOCAL_MOUNT/tnt/$HOST_TARGET"

echo "Using HOST_TARGET=$HOST_TARGET"

setenv TSP_BASE $HOST_BASE/tsp
echo "Using TSP_BASE=$TSP_BASE"

##############################
# HOME_EXEC_BASE / HOME_EXEC_CURRENT
##############################

setenv HOME_EXEC_BASE $TSP_BASE/exec
setenv HOME_EXEC_CURRENT $HOME_EXEC_BASE/current


##############################
# Paths
##############################


setenv PATH ${TSP_BASE}/src/scripts:${HOME_EXEC_CURRENT}/bin.consumer.debug:${HOME_EXEC_CURRENT}/bin.provider.debug:${TSP_BASE}/src/tsp/tests/etape1:${PATH}



##############################
# DEVBASE
##############################


setenv DEVBASE $TSP_BASE
echo "Using DEVBASE=$DEVBASE"

if ( ! $?STRACE_DEBUG ) then 
	setenv STRACE_DEBUG 3
endif
echo "Using STRACE_DEBUG=$STRACE_DEBUG"


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

alias go_ec 'cd ${HOME_EXEC_CURRENT}/bin.consumer'
alias go_ecd 'cd ${HOME_EXEC_CURRENT}/bin.consumer.debug'
alias go_ep 'cd ${HOME_EXEC_CURRENT}/bin.provider'
alias go_epd 'cd ${HOME_EXEC_CURRENT}/bin.provider.debug'
alias go_tsp 'cd ${TSP_BASE}'
alias go_scripts 'cd ${TSP_BASE}/src/scripts'




##############################
# CVS 
##############################
unsetenv CVSROOT
alias lcvs "cvs -d $LOCAL_MOUNT/tnt/"
alias gcvs "cvs -d $LOCAL_MOUNT/dev442/"





