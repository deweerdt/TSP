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
    set SPARC_TYPE=` isalist | awk '{ print $1 }' | grep sparcv9 `  
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


setenv PATH ${PATH}:${HOME_EXEC_CURRENT}/bin.consumer:${HOME_EXEC_CURRENT}/bin.provider:${TSP_BASE}/src/tsp/tests/etape1:${TSP_BASE}/src/scripts:${TSP_BASE}/src/tsp/tests/stage2:${TSP_BASE}/src/tsp/tests/stage3



##############################
# Misc.
##############################

if ( ! $?STRACE_DEBUG ) then 
	setenv STRACE_DEBUG 2
endif
echo "Using STRACE_DEBUG=$STRACE_DEBUG"

##############################
# Navigation
##############################
alias go_ec 'cd ${HOME_EXEC_CURRENT}/bin.consumer'
alias go_ecd 'cd ${HOME_EXEC_CURRENT}/bin.consumer.debug'
alias go_ep 'cd ${HOME_EXEC_CURRENT}/bin.provider'
alias go_epd 'cd ${HOME_EXEC_CURRENT}/bin.provider.debug'
alias go_tsp 'cd ${TSP_BASE}'
alias go_scripts 'cd ${TSP_BASE}/src/scripts'
alias go_tests 'cd ${TSP_BASE}/src/tsp/tests'


##############################
# CVS 
##############################
unsetenv CVSROOT 
alias lcvs "cvs -d $LOCAL_MOUNT/tnt/CVSROOT"
alias gcvs "cvs -d $LOCAL_MOUNT/dev442/CVSROOT"







