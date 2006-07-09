#!/bin/sh
find $1 -type d -name CMakeFiles -exec rm -rf {} \; -prune
