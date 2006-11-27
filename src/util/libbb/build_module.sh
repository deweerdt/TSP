#!/bin/sh
# This helper script switches Makefiles between the one needed for the 
# module and the "main" one
cp Makefile Makefile.us && cp Makefile.k Makefile && make && mv Makefile.us Makefile
