#!/bin/sh

# This helper script creates a kbuild buiding environment... fill it
# with whatever is needed before building

SRC_DIR=$1
DST_DIR=$2

mkdir -p $DST_DIR/kbuild
cp $SRC_DIR/Makefile.k $DST_DIR/kbuild/Makefile
cp $SRC_DIR/bb_alias.h $SRC_DIR/bb_utils.h $SRC_DIR/bb_core.h $SRC_DIR/bb_core_k.h $SRC_DIR/bb_simple.h $DST_DIR/kbuild/
cp $SRC_DIR/bb_core.c $SRC_DIR/bb_alias.c $SRC_DIR/bb_core_k.c $SRC_DIR/bb_module.c $SRC_DIR/bb_simple.c $DST_DIR/kbuild/
make -C $DST_DIR/kbuild
