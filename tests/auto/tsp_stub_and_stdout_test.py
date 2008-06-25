#!/usr/bin/env python

import logging
import os
import time
import threading
import getopt, sys
import dtest

def usage():
    print "Usage:\n %s --provider=<user>@[<host>]:<stub_path> --consumer=<user>@[<host>]:<stdout_path>" % sys.argv[0]

def getUserHostPath(argument):
    if argument.find("@") != -1:
        (user,argument) = argument.split("@",1)
    else:
        user = os.environ["USER"]
    if argument.find(":") != -1:
        (host,path) = argument.split(":",1)
    else:
        host = "localhost"
        path = argument
    retval = dict()
    retval['user'] = user
    retval['host'] = host
    retval['path'] = path
    return retval

try:
    opts, args = getopt.getopt(sys.argv[1:], "p:c:", ["provider=", "consumer="])
except getopt.GetoptError, err:
    print >> stderr, "opt = %s, msg = %s" % (err.opt,err.msg)
    usage()
    sys.exit(2)

if len(opts) < 2:
    usage()
    sys.exit(2)

for o, a in opts:
    if o in ("-p", "--provider"):
        stub_param   = getUserHostPath(a)
    if o in ("-c", "--consumer"):
        stdout_param = getUserHostPath(a)

stdout = dtest.DTester("tsp_stdout",
                       session=dtest.SSHSessionHandler(stdout_param['user'],host=stdout_param['host'])) 
stub = dtest.DTester("tsp_stubbed_server",
                     session=dtest.SSHSessionHandler(stdout_param['user'],host=stdout_param['host']))
# you may change the default time out value
stub.timeout = 8
# you add want to save the output of your dtester to a file.
stub.stdout = file(stub.name + ".out",'w+')
stub.stdin  = file(stub.name + ".in",'w+')
stdout.stdout = file(stdout.name + ".out",'w+')
#stdout.stdin  = file(stdout.name + ".in",'w+')

dtest.DTester.logger.setLevel(level=logging.WARNING)

stub.addRunStep("ok",True,"TSP Stub and Stdout Starts")
stub.addRunStep("runCommand",command=stub_param['path'])
stub.addRunStep("expectFromCommand",pattern="TSP Provider on PID")
stub.addRunStep("barrier","provider started")
stdout.addRunStep("barrier","provider started")
stdout.addRunStep("runCommand",command=stdout_param['path']+" -u rpc://"+stub.session.host)
stdout.addRunStep("expectFromCommand",pattern="tsp_stdout_client: Using provider URL")
stdout.addRunStep("expectFromCommand",pattern="End of Test OK")
stdout.addRunStep("ok",stdout.getFutureLastStepStatus,"First stdout run (no args)")
stdout.addRunStep("terminateCommand")
stdout.addRunStep("waitCommandTermination")
stdout.addRunStep("runCommand",command=stdout_param['path']+" -n 150 -u rpc://"+stub.session.host)
stdout.addRunStep("expectFromCommand",pattern="tsp_stdout_client: Using provider URL")
stdout.addRunStep("expectFromCommand",pattern="End of Test OK")
stdout.addRunStep("ok",stdout.getFutureLastStepStatus,"Second stdout run (-n 150) ")
stdout.addRunStep("terminateCommand")
stdout.addRunStep("waitCommandTermination")
stdout.addRunStep("barrier","consumer ended",timeout=1)
stub.addRunStep("barrier","consumer ended")
stub.addRunStep("terminateCommand")
stub.addRunStep("waitCommandTermination")
stub.addRunStep("ok",True,"TSP Stub and Stdout Ends")

# Here begins the test
dtest.DTestMaster.logger.setLevel(level=logging.WARNING)
dtest.DTester.logger.setLevel(level=logging.WARNING)
dtest.SSHSessionHandler.logger.setLevel(level=logging.WARNING)

def goTest():
    myDTestMaster = dtest.DTestMaster("TSP Stub Server/Stdout Test",description="This TSP test sequence launch  a stubbed server and an stdout client")
    myDTestMaster.timeout = 40
    myDTestMaster.register(stdout)
    myDTestMaster.register(stub)
    myDTestMaster.startTestSequence()
    myDTestMaster.waitTestSequenceEnd()
    
goTest()

