#!/bin/sh
# This helper script switches Makefiles between the one needed for the 
# module and the "main" one

SRC_DIR=$1
DST_DIR=$2

rm -rf $DST_DIR/kbb_debian_build
mkdir -p $DST_DIR/kbb_debian_build
cp $DST_DIR/Makefile.k $DST_DIR/kbb_debian_build/Makefile
cp $SRC_DIR/bb_alias.h $SRC_DIR/bb_utils.h $SRC_DIR/bb_core.h $SRC_DIR/bb_core_k.h $SRC_DIR/bb_simple.h $SRC_DIR/bb_local.h $SRC_DIR/bb_callback.h $DST_DIR/kbb_debian_build/
cp $SRC_DIR/bb_core.c $SRC_DIR/bb_alias.c $SRC_DIR/bb_core_k.c $SRC_DIR/bb_module.c $SRC_DIR/bb_simple.c $SRC_DIR/bb_local.c $SRC_DIR/bb_callback.c $DST_DIR/kbb_debian_build/
cp -r kbb_debian $DST_DIR/kbb_debian_build/debian
cp -r $DST_DIR/debian_changelog $DST_DIR/kbb_debian_build/debian/changelog
cd $DST_DIR/kbb_debian_build
fakeroot dpkg-buildpackage -us -uc
