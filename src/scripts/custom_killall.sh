#!/bin/sh
# show up script version

version ()
{
    printf "\n%s\n" "TSP Custom killall script[sh] - V1.0"
}

# Waiting for time out
pids=`ps -edf | egrep -v "grep|killall" | grep $1  | awk '{ print $2 }'`
if [ -n "$pids" ];  then
      kill  $pids
      sleep 1
fi


