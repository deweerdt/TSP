# -*- Makefile -*- 

# Makeparams.java

include $(DEVBASE)/make/Makeproject

# Default tools
# -------------
# Using standard tools
JAVA = java
JAVAC = javac
# Use RemoteTea Java RPCGEN (see: http://acplt.plt.rwth-aachen.de/ks/english/remotetea.html)
# Customize your installation of RemoteTea
JRPCBASE = $(DEVBASE)/../RemoteTea
JRPCGENJAR = $(JRPCBASE)/classes/jrpcgen.jar
JRPCJAR = $(JRPCBASE)/classes/oncrpc.jar
JRPCPORTMAPJAR = $(JRPCBASE)/classes/jportmap.jar
JRPCGEN = $(JAVA) -jar $(JRPCGENJAR)
JAR = jar

# Default options
# ---------------
CLASSPATH = 
JARFLAGS =
JFLAGS = 

#include .make.state