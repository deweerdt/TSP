#!/bin/sh
FILTER_CVSDIR="s/^CVS.*//"
FILTER_TILDE="s/.*~//"
# visual command used for diffing files
VISUAL_DIFF=tkdiff

# Affiche la version du script
version ()
{
    printf "\n%s\n" "psinfo V1.0 - E.N. - 05/10/2004"
    printf "%s\n" "Find and print information about a process"    
} # end of version

# Affiche l'utilisation
usage ()
{
    version
    printf "%s\n" "Usage: `basename $0` <processname>"
    
} # end of usage

infosexec ()
{
    DATE=`date`
    WHO=`whoami`
    MACHINE=`uname -n`
    printf "%s" "Compare dir1=$1 versus dir2=$2"
} # end of infoexec

###### main begins ########

if [ $# -lt  1 ];
then
    usage
    exit 1
fi

version

PTHREAD_NPTL=`getconf GNU_LIBPTHREAD_VERSION | grep -i nptl`
if [ -n "$PTHREAD_NPTL" ]; 
then
   printf "Thread system used : %s\n"  "$PTHREAD_NPTL"
else 
   printf "Thread system used : %s\n"  "LinuxThread"
fi

# find the process
PID=`ps -ea | grep "$1" | grep -v "grep" | awk '{print $1}' `
if [ "$PID" = "" ];
then
  printf "No process named <%s> found.\n" $1
  exit 1
else
  printf "Process <%s> found, PID is <%s>\n" $1 $PID
fi

if [ -n "$PTHREAD_NPTL" ];
then
   NTHREAD=`ps -Tp $PID | wc -l`
   #remove status line count
   NTHREAD=`expr $NTHREAD - 1`
else 
   NTHREAD=`ps -ef | grep $PID`
fi
   printf "Process contains <%s> thread(s) (including main)\n" $NTHREAD
   printf "%s\n" "------------ Detailed thread info ------------"
if [ -n "$PTHREAD_NPTL" ];
then
   ps -Tcp $PID
else
   ps -ec | grep $PID
fi
   printf "%s\n" "------------ ******************** ------------"




