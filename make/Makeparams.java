# -*- Makefile -*- 
# Makeparams.java

include $(DEVBASE)/make/Makeparams

# target directory
# ----------------
#DIREXEC= $(DIRBASE)/$(HOST_TARGET)/$(DEBUG_MODE)
#DIRBIN = $(DIREXEC)/bin
#DIRLIB = $(DIREXEC)/lib


# Default tools
# -------------
# Using standard tools
JAVA = java
JAVAC = javac
JAR = jar

JARDIR= $(DEVBASE)/external

# RemoteTea Java RPCGEN (http://acplt.plt.rwth-aachen.de/ks/english/remotetea.html)
# Customize your installation of RemoteTea
JRPCBASE = $(JARDIR)/RemoteTea
JRPCGENJAR = $(JRPCBASE)/jrpcgen.jar
JRPCJAR = $(JRPCBASE)/oncrpc.jar
JRPCPORTMAPJAR = $(JRPCBASE)/jportmap.jar
JRPCGEN = $(JAVA) -jar $(JRPCGENJAR)

# Default options
# ---------------
CLASSPATH = 
JARFLAGS =
JFLAGS = 

#include .make.state
