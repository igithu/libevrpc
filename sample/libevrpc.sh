#!/bin/sh
#
# chkconfig: 2345  80 50
# description: data_server is for testing how to write service in Linux 
#              
# processname: data_server
# 
# Source function library.
. /etc/rc.d/init.d/functions

ret=0

start() {
    # check fdb status
    echo "start data_server"
    ./data_server &
    ret=$?
} 

stop() {
    echo "stop data_server" 
    kill -9 $(ps -ef | grep data_server | grep -v grep | awk '{print $2}')
    ret=$?
} 

status() {
    local result
    echo "check status of data_server..."
    result=$( ps -ef | grep -v data_server | grep -v grep | wc -l )
    if [ $result -gt 0 ] ; then
        echo "data_server is up"
        ret=0
    else
        echo "data_server is down"
        ret=1
    fi
    echo "check status of data_server...done."
} 

# See how we were called.
case "$1" in
  start)
        start
        ;;
  stop)
        stop
        ;;
  status)
        status 
        ;;
  restart)
        stop
        start
        ;;
  *)
        echo $"Usage: $0 {start|stop|status}"
        exit 1
esac

exit $ret
