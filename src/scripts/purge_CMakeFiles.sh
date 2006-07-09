#!/bin/sh
find $1 -type d -name CMakeFiles -exec rm -rf {} \; -prune
find $1 -type f -name cmake_install.cmake -exec rm -f {} \; -prune