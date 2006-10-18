/*

$Header: /home/def/zae/tsp/tsp/external/TspWin32/getopt.h,v 1.1 2006-10-18 08:27:56 erk Exp $

-----------------------------------------------------------------------

 * Here's something you've all been waiting for:  the AT&T public domain
 * source for getopt(3).  It is the code which was given out at the 1985
 * UNIFORUM conference in Dallas.  I obtained it by electronic mail
 * directly from AT&T.  The people there assure me that it is indeed
 * in the public domain.
 *
 * There is no manual page.  That is because the one they gave out at
 * UNIFORUM was slightly different from the current System V Release 2
 * manual page.  The difference apparently involved a note about the
 * famous rules 5 and 6, recommending using white space between an option
 * and its first argument, and not grouping options that have arguments.
 * Getopt itself is currently lenient about both of these things White
 * space is allowed, but not mandatory, and the last option in a group can
 * have an argument.  That particular version of the man page evidently
 * has no official existence, and my source at AT&T did not send a copy.
 * The current SVR2 man page reflects the actual behavor of this getopt.
 * However, I am not about to post a copy of anything licensed by AT&T.

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium.eads.net
Component : TspWin32

-----------------------------------------------------------------------

Purpose   : Emulation of getopt UNIX function under Windows 
-----------------------------------------------------------------------
 */
#ifndef __GETOPT_H
#define __GETOPT_H

#include "TspWin32.h"

_EXPORT_TSPWIN32 char *optarg;
_EXPORT_TSPWIN32 int optind, opterr;

_EXPORT_TSPWIN32 int getopt (int argc, char **argv, const char *options);

#endif