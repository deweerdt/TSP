#!/usr/local/bin/tcsh
cvs status -v | nawk -f ${DEVBASE}/src/scripts/check_tag.awk
