#!/bin/sh
#========================================
# TSP pluggins in jSynoptic
# Allow to plot symbols in java GUI
#========================================

JAR_DIR=${DEVBASE}/external
JSYN_DIR=${JAR_DIR}/jSynoptic
EXEC_DIR=${DEVBASE}/exec/DEV/java

java -classpath ${EXEC_DIR}/jsynoptic-tsp-plugin.jar:${JAR_DIR}/RemoteTea/oncrpc.jar -jar ${JSYN_DIR}/jsynoptic.jar

