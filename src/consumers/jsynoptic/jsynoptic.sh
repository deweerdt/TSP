#!/bin/sh
#========================================
# TSP pluggins in jSynoptic
# Allow to plot symbols in java GUI
#========================================

export JAR_DIR=${DEVBASE}/external
export JSYN_DIR=${JAR_DIR}/jSynoptic
export EXEC_DIR=${DEVBASE}/exec/DEV/java/classes

java -classpath ${EXEC_DIR}:${JSYN_DIR}/jcommon.jar:${JSYN_DIR}/jfreechart.jar:${JSYN_DIR}/jsynoptic.jar:${JAR_DIR}/RemoteTea/oncrpc.jar:${JSYN_DIR}/simtools.jar jsynoptic.ui.Run

