#!/bin/sh
##############################
# Find host type
##############################

HOST_UNAME=`uname -a | awk '{ print $1 }'`

#Linux
if [ "$HOST_UNAME" = "Linux" ]; then
    export HOST_TARGET="linux"
#Sun & Sun64
elif [ "$HOST_UNAME" = "SunOS" ]; then
    #try to find isalist util
    SPARC_TYPE=` isalist | awk '{ print $1 }' | grep sparcv9`
    if [ $? = 0 ]; then
	if [ -n $SPARC_TYPE ]; then
	    export HOST_TARGET="sun64"
	else
	    export HOST_TARGET="sun"
	fi	
    else
	export HOST_TARGET="sun"
    fi
#dec
elif [ "$HOST_UNAME" = "OSF1" ]; then
     export HOST_TARGET="dec"
#???
else
     echo "ERROR : Unknown system : $HOST_UNAME"
fi


# %%tsp_base%% will be set by the configure script 
export TSP_BASE=%%tsp_base%%
echo "Using TSP_BASE=$TSP_BASE"

##############################
# HOME_EXEC_BASE / HOME_EXEC_CURRENT
##############################

export HOME_EXEC_BASE=$TSP_BASE/exec
export HOME_EXEC_CURRENT=$HOME_EXEC_BASE/current


##############################
# Paths
##############################


export PATH=${TSP_BASE}/src/scripts:${HOME_EXEC_CURRENT}/bin.consumer.debug:${HOME_EXEC_CURRENT}/bin.provider.debug:${TSP_BASE}/src/core/tests/stage1:${TSP_BASE}/src/core/tests/stage2:${TSP_BASE}/src/core/tests/stage3:${PATH}


##############################
# DEVBASE
##############################


export DEVBASE=$TSP_BASE
echo "Using DEVBASE=$DEVBASE"

if [ -z "$STRACE_DEBUG" ]; then 
    export STRACE_DEBUG=3
fi
echo "Using STRACE_DEBUG=$STRACE_DEBUG"


alias go_ec='cd ${HOME_EXEC_CURRENT}/bin.consumer'
alias go_ecd='cd ${HOME_EXEC_CURRENT}/bin.consumer.debug'
alias go_ep='cd ${HOME_EXEC_CURRENT}/bin.provider'
alias go_epd='cd ${HOME_EXEC_CURRENT}/bin.provider.debug'
alias go_tsp='cd ${TSP_BASE}'
alias go_scripts='cd ${TSP_BASE}/src/scripts'
alias go_tests='cd ${TSP_BASE}/src/core/tests'






