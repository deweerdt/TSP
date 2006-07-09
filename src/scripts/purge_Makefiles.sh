#!/bin/sh
find $1 -type f -name Makefile -exec rm -rf {} \; -prune
