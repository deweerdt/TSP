##############################
# Introspection
##############################

set HOST_UNAME=`uname -a | awk '{ print $1 }'`

if ( "$HOST_UNAME" == "Linux" )  then
	setenv HOST_TARGET "linux"
else if ( "$HOST_UNAME" == "SunOS") then
      setenv HOST_TARGET "sun"
else if ( "$HOST_UNAME" == "OSF1") then
      setenv HOST_TARGET "dec"
else
     echo "ERROR : Unknown system : $HOST_UNAME"
     exit -1
endif


if ( "$HOST_TARGET" != "dec" ) then
	setenv HOST_BASE "/home2/breiz/tnt/$HOST_TARGET"
else
	setenv HOST_BASE "/net/breiz/home2/breiz/tnt/dec"
endif


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


setenv PATH ${PATH}:${HOME_EXEC_CURRENT}/bin.consumer:${HOME_EXEC_CURRENT}/bin.provider:${TSP_BASE}/src/tsp/tests/etape1:${TSP_BASE}/src/scripts



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


##############################
# CVS 
##############################
setenv CVSROOT /home2/breiz/tnt/CVSROOT
alias lcvs 'cvs -d /home2/breiz/tnt/CVSROOT '
alias gcvs 'cvs -d /home2/breiz/dev442/CVSROOT'







