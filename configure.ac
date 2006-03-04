#
# Process this file with autoconf to produce a configure script.
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# THIS FILE NEED TO BE PROCESSED WITH AUTOCONF
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# 
AC_INIT([the Transport Sampling Protocol], [0.7.3cvs], [tsp@astrium.eads.net], [tsp])

echo "**********************************************************"
echo "*  TSP: AC_PACKAGE_NAME, version AC_PACKAGE_VERSION       "
echo "*  see http://savannah.nongnu.org/projects/tsp            "
echo "*  or mail to AC_PACKAGE_BUGREPORT                        "
echo "**********************************************************"

# Check for a required file
AC_CONFIG_SRCDIR([src/core/driver/tsp_consumer.c])
#AC_CONFIG_HEADER([src/core/include/tsp_prjcfg.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# setup AR for the TSP host
AC_PROG_RANLIB
AC_CHECK_PROG(AR, ar, ar, ;)
AR="$AR"


ENABLED_TSP_TARGET="host"

##########################
# Setup C part of TSP
##########################
AC_LANG(C)
### You may force the C compiler to be used by configure ###
AC_ARG_WITH(host-cc,         [  --with-host-cc=C_COMPILER         chose the TSP host C compiler],
            CC=$withval,)
AC_ARG_ENABLE(ctsp,   [  --enable-ctsp                enable C TSP library build],
	      CTSP=$enableval,
	      CTSP="yes")
AC_MSG_CHECKING(C part of the TSP)
if test "$CTSP" = "yes"; then
AC_MSG_RESULT([Enabled, will be configured])
AC_PROG_CC(gcc cc icc kcc)
else
AC_MSG_RESULT([Disabled, won't be configured])
fi


####################################################################
# C part has been enabled verify libraries, headers and functions 
####################################################################
if test "$CTSP" = "yes"; then
# Checks for libraries.
# Check for pthread (-lpthread)
AC_HAVE_LIBRARY([pthread],,AC_MSG_WARN([Won't be able to build C version of TSP]))
# Check for math (-lm) for src/util/libcalc
AC_HAVE_LIBRARY([m],,AC_MSG_WARN([Won't be able to build src/util/libcalc]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h errno.h fcntl.h memory.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/param.h sys/socket.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([bzero gethostbyname gethostname gethrtime gettimeofday memset munmap socket strdup strstr strtol])
fi
##########################
### End of C Setup     ###
##########################

####################################
# PKG config generic config script.
# is pkg-config present ?
####################################
AC_CHECK_PROG([PKG_CONFIG],[pkg-config],[`which pkg-config`],[notfound],[$PATH],[rej])
AC_MSG_CHECKING([pkg-config setup verification...])
if test "$PKG_CONFIG" = "notfound"; then
  AC_MSG_RESULT([NOK: pkg-config script (generic pkg package) not found will use specific script (if any)])
  PKG_CONFIG_USER="no"
else
  AC_MSG_RESULT([Ok])
  PKG_CONFIG_USER="yes"
fi

################################
# XML config reader.
# is xml2-config present ?
# moreover
# is xmlwriter supported ?
################################

verify_version()
{
    X=`echo $1 | awk -F . '{print $1}'`
    Y=`echo $1 | awk -F . '{print $2}'`
    Z=`echo $1 | awk -F . '{print $3}'`
    Xr=`echo $2 | awk -F . '{print $1}'`
    Yr=`echo $2 | awk -F . '{print $2}'`
    Zr=`echo $2 | awk -F . '{print $3}'`
    if test $X -lt $Xr ;
    then
       echo "NOK"
       return
    else
       if test $X -gt  $Xr ;
       then
         echo "OK"
         return
       fi
    fi
    # Now $X == $r
    if test $Y -lt $Yr ;
    then
      echo "NOK"
      return
    else
      if test $Y -gt  $Yr ;
       then
         echo "OK"
         return
       fi
    fi
    # Now $X == $XR and $Y == $Yr
    if test $Z -ge $Zr ;
    then
      echo "OK"
    else
      echo "NOK"
      return
    fi
} # end of verify_version


AC_CHECK_PROG([XML2_CONFIG],[xml2-config],[`which xml2-config`],[notfound],[$PATH],[rej])
AC_MSG_CHECKING([lib XML2 setup verification...])
if test "$XML2_CONFIG" = "notfound"; then
  AC_MSG_RESULT([NOK: xml2-config script (xml2-devel package) not found TSP consumer using XML config files are DISABLED])
  XML2_CONFIG_USER="no"
else	
  AC_MSG_RESULT([Ok])
  XML2_CONFIG_USER="yes"
  # Now check for XMLWRITER API
  AC_MSG_CHECKING([libxml2 version])
  XML2_VERSION=[`xml2-config --version`]
  AC_MSG_RESULT([$XML2_VERSION])
  AC_MSG_CHECKING([xmlwriter API support])
  XML2_VERSION_REQUIRED="2.6.0"
  XMLWRITER_OK=`verify_version $XML2_VERSION $XML2_VERSION_REQUIRED `
  if test "$XMLWRITER_OK" = "OK"; then
     XMLWRITER_SUPPORTED="yes"
     AC_MSG_RESULT([OK ($XML2_VERSION > $XML2_VERSION_REQUIRED)])
  else 
     XMLWRITER_SUPPORTED="no"
     AC_MSG_RESULT([NOK ($XML2_VERSION < $XML2_VERSION_REQUIRED), save config for gdisp+ DISABLED])
     AC_MSG_WARN([Upgrade your libxml2 (version >= $XML2_VERSION_REQUIRED) if you want save config for gdisp+ to be enabled])
  fi
fi

################################
### End of XML2 devel check  ###
################################

################################
# GTK based consumer.
# is gtk-config present ?
################################
AC_CHECK_PROG([GTK_CONFIG],[gtk-config],[`which gtk-config`],[notfound],[$PATH],[rej])
AC_MSG_CHECKING([GTK setup verification...])
if test "$GTK_CONFIG" = "notfound"; then
  AC_MSG_RESULT([NOK: gtk-config script (gtk-devel package) not found GTK based TSP consumer are DISABLED])
  GTK_BASED_CONSUMER="no"
else
  AC_MSG_RESULT([Ok])
  GTK_BASED_CONSUMER="yes"
fi
################################
### End of GTK devel check   ###
################################

################################
# Flex/Bison based config file
# (tsp_ascii_writer)
################################
AC_PROG_LEX
if test "x$LEX" = "x"; then
  LEX="notfound"
  AC_SUBST(LEX_OUTPUT_ROOT, lex.yy)
  AC_SUBST(LEXLIB, '')
fi

AC_PROG_YACC

if test "x$YACC" = "x"; then 
   YACC="notfound"
fi

AC_MSG_CHECKING([Lex/Yacc setup verification...])
if test "$LEX" = "notfound"; then
  AC_MSG_RESULT([NOK: [f]lex not found, lex/yacc based parser disabled (so is tsp_ascii_writer consumer)])
  LEX_YACC_PARSER="no"
elif test "$YACC" = "notfound"; then
  AC_MSG_RESULT([NOK: yacc/bison not found, lex/yacc based parser disabled (so is tsp_ascii_writer consumer)])
else
  AC_MSG_RESULT([Ok, lex=$LEX yacc=$YACC])
  LEX_YACC_PARSER="yes"
fi

################################
### End of GTK devel check   ###
################################

######################################
# shared library handling
######################################
AC_ARG_ENABLE(sharedlib,[  --enable-sharedlib           enable shared libraries (this feature is ALPHA)],
	      SHAREDLIB_ENABLED=$enableval,
	      SHAREDLIB_ENABLED="no")
AC_MSG_CHECKING([enabling shared library use])
if test "$SHAREDLIB_ENABLED" = "yes"; then
  AC_MSG_RESULT([Enabled, will be configured])
else
  AC_MSG_RESULT([Disabled, won't be configured])
fi
#######################################
### End shared libraries
#######################################

######################################
# OpenGL part for TSP consumers
######################################
AC_ARG_ENABLE(openGL,   [  --enable-openGL              enable openGL use for TSP consumers  (this feature is ALPHA)],
	      OPENGLOK=$enableval,
	      OPENGLOK="yes")
AC_MSG_CHECKING([enabling OpenGL use])
if test "$OPENGLOK" = "yes"; then
  AC_MSG_RESULT([Enabled, will be configured])
  AC_CHECK_HEADERS([GL/gl.h GL/glu.h],[GL_OK="yes"],[GL_OK="no"])
  AC_MSG_CHECKING([usable OpenGL installation])
  if test "$GL_OK" = "yes"; then
     AC_MSG_RESULT([OK, Basic OpenGL headers ok will enable openGL use in TSP])
  else 
     AC_MSG_RESULT([NOK, Basic OpenGL headers NOT disabling openGL use in TSP])
  fi
  AC_CHECK_HEADERS([GL/glut.h],[GLUT_OK="yes"],[GLUT_OK="no"])
  AC_MSG_CHECKING([usable OpenGL/GLUT installation])
  if test "$GLUT_OK" = "yes"; then
     AC_MSG_RESULT([OK, GLUT OpenGL headers ok will enable GLUT openGL use in TSP])
  else 
     AC_MSG_RESULT([NOK, GLUT OpenGL headers NOT disabling GLUT openGL use in TSP])
  fi
else
  AC_MSG_RESULT([Disabled, won't be configured])
fi
#######################################
### End OpenGL part for TSP consumers
#######################################

######################################
# Build XMLRPC for command transport
######################################
AC_ARG_ENABLE(xmlrpc,   [  --enable-xmlrpc              enable xmlrpc for TSP commands  (this feature is ALPHA and _disables_ RPC usage)],
	      BUILD_XMLRPC=$enableval,
	      BUILD_XMLRPC="no")
if test "$BUILD_XMLRPC"  = "yes"; then
	AC_CHECK_PROG([XMLRPC_CONFIG],[xmlrpc-c-config],[`which xmlrpc-c-config`],[notfound],[$PATH],[rej])
	if test "$XMLRPC_CONFIG" = "notfound"; then
	  AC_MSG_RESULT([NOK: xmlrpc-c-config script (xmlrpc-c-devel package) not found we'll use RPC instead])
	  BUILD_XMLRPC="no"
	else
	  AC_MSG_RESULT([Ok])
	  AC_MSG_CHECKING([enabling XMLRPC usage (disabling RPC)])
	  BUILD_XMLRPC="yes"
    fi    
fi
#######################################
### End XMLRPC for command transport
#######################################

################################
# BlackBoard part of TSP
################################
AC_ARG_ENABLE(blackboard,   [  --enable-blackboard          enable BlackBoard library and TSP BlackBoard Provider],
	      BB=$enableval,
	      BB="yes")
AC_MSG_CHECKING([BlackBoard part of the TSP])
if test "$BB" = "yes"; then
AC_MSG_RESULT([Enabled, will be configured])
else
AC_MSG_RESULT([Disabled, won't be configured])
fi
################################
### End of BlackBoard Setup  ###
################################

###############################################
# Scripting API for TSP
# this is an alpha feature which will
# make the TSP usable from srcipting language
# like Tcl or Python.
###############################################
AC_ARG_ENABLE(script-binding,   [  --enable-script-binding      enable scripting language binding of the TSP (this feature is ALPHA)],
	      SCRIPT_BINDING=$enableval,
	      SCRIPT_BINDING="no")
AC_MSG_CHECKING([Scripting binding for the TSP])
if test "$SCRIPT_BINDING" = "yes"; then
AC_MSG_RESULT([Enabled, will be configured])
else
AC_MSG_RESULT([Disabled, won't be configured])
fi
################################
### End of BlackBoard Setup  ###
################################

##########################
# Setup C++ part of TSP
##########################
### You may force the C compiler to be used by configure ###
AC_ARG_WITH(host-cxx,        [  --with-host-cxx=CXX_COMPILER      chose the TSP host C++ compiler],
            CXX=$withval,)

AC_ARG_ENABLE(cxxtsp, [  --enable-cxxtsp              enable TSP C++ binding],
	      CXXTSP=$enableval,
	      CXXTSP="no")

AC_MSG_CHECKING([C++ binding the TSP])
if test "$CXXTSP" = "yes"; then
AC_MSG_RESULT([Enabled, will be configured])
AC_LANG_PUSH(C++)
AC_PROG_CXX(g++ CC)
AC_LANG_POP(C++)
else
AC_MSG_RESULT([Disabled, won't be configured])
fi

##########################
### End of C++ Setup   ###
##########################

#####################|##|############################|############
# Get the system type from AC
#####################|##|############################|############
AC_CANONICAL_HOST

AC_MSG_CHECKING([for TSP host target])

ARFLAGS="rvs"

case "$host" in
    *linux*)	
	AC_MSG_RESULT([Linux is OK])
	HOST_TARGET=linux
	LDLIBS="-lm  -lpthread"
	CPPFLAGS="-D_REENTRANT  -DUSE_STRACE -DTSP_NO_XDR_ENCODE -Wall"
        ;;
    *openbsd*)
	AC_MSG_RESULT([OpenBSD is OK])	
	HOST_TARGET=openbsd
	LDLIBS="-lsocket -lm -lposix4 -lgen -lpthread"
        ;;
    i386-pc-solaris*)
	AC_MSG_RESULT([i386 Solaris is OK])	
	CPPFLAGS="-D__EXTENSIONS__ -D_REENTRANT  -DUSE_STRACE -DTSP_NO_XDR_ENCODE -Wall"
	LDLIBS="-lsocket -lnsl -lm -lposix4 -lgen -lpthread"
	HOST_TARGET=sun
        ;;
    sparc-sun-solaris*)
	AC_MSG_RESULT([Sparc Solaris is OK])	
	CPPFLAGS="-D__EXTENSIONS__ -D_REENTRANT  -DUSE_STRACE -DTSP_NO_XDR_ENCODE -Wall"
	LDLIBS="-lsocket -lnsl -lm -lposix4 -lgen -lpthread"
	HOST_TARGET=sun
        ;;
    sparc-sun-sunos*)
	AC_MSG_RESULT([Sparc SunOS is OK])
	CPPFLAGS="-D__EXTENSIONS__ -D_REENTRANT  -DUSE_STRACE -DTSP_NO_XDR_ENCODE -Wall"
	LDLIBS="-lsocket -lnsl -lm -lposix4 -lgen -lpthread"
	HOST_TARGET=sun
        ;;
    *osf*)
	AC_MSG_RESULT([OSF1 Host is OK])
	CPPFLAGS="-D__EXTENSIONS__ -D_REENTRANT  -DUSE_STRACE -DTSP_NO_XDR_ENCODE -Wall"
	LDLIBS="-lsocket -lnsl -lm -lposix4 -lgen -lpthread"
	HOST_TARGET=dec
	;;
    *powerpc-unknown-none*)
	AC_MSG_RESULT([ppc is OK])
	HOST_TARGET=ppc
	LDLIBS="-lm  -lpthread"
	CPPFLAGS="-D_REENTRANT  -DUSE_STRACE -DTSP_NO_XDR_ENCODE -Wall"
	HOST_TARGET=ppc
	;;
	*)
	AC_MSG_WARN([$host has NOT been tested, be warned!!])
	HOST_TARGET=unknown
	;;
esac
# enable the HOST_TARGET
ENABLED_TSP_TARGET=`echo $ENABLED_TSP_TARGET | sed -e "s/host/$HOST_TARGET/g"`

############################################################################
# Verify enabled target
# --enable-tsp-target-<targetname>
# --with-tsp-target-<targetname>-cc
# vxworks
# the host target (linux, sun, sun64, dec) is automagically enabled
# the vxworks target is enabled by the enable-vxworks, and disable others
############################################################################
AC_ARG_ENABLE(tsp-target-vxworks,   [  --enable-tsp-target-vxworks  enable VxWorks TSP target],
	      TSPTARGET_VXWORKS=$enableval,
	      TSPTARGET_VXWORKS="no")

AC_ARG_WITH(tsp-target-vxworks-cc,  [  --with-tsp-target-vxworks-cc=C_CROSS_COMPILER     chose the TSP target vxworks C cross compiler],
	TSPTARGET_VXWORKS_CC=$withval,
	TSPTARGET_VXWORKS_CC="no")

############################################################################
# Map enabled target onto consumer/provider part
############################################################################

AC_MSG_NOTICE([Enabled TSP target are <$ENABLED_TSP_TARGET>])


AC_ARG_ENABLE(tsp-consumer,   [  --enable-tsp-consumer        map TSP target to the TSP consumer],
	      TSPCONSUMER=$enableval,
	      TSPCONSUMER="yes")

AC_ARG_ENABLE(tsp-provider,   [  --enable-tsp-provider        map TSP target to the TSP provider],
	      TSPPROVIDER=$enableval,
	      TSPPROVIDER="yes")

############################################################################
# Choice of compile mode : debug, opt or both
############################################################################
AC_ARG_ENABLE(tsp-build-debug,   [  --enable-tsp-build-debug     select build mode debug],
	      TSPBUILD_DEBUG=$enableval,
	      TSPBUILD_DEBUG="yes")

AC_ARG_ENABLE(tsp-build-opt,   [  --enable-tsp-build-opt       select build mode optimized],
	      TSPBUILD_OPT=$enableval,
	      TSPBUILD_OPT="no")

if test "$TSPBUILD_DEBUG" = "yes" ; then
	if test "$TSPBUILD_OPT" = "yes" ; then
		TSPBUILD="debug:opt"
	else
		TSPBUILD="debug"
	fi
else
	if test "$TSPBUILD_OPT" = "yes" ; then
		TSPBUILD="opt"
	else
		echo "Need to compile etiher debug or opt mode. Debug is selected "
		TSPBUILD="debug"
	fi
fi

############################################################################
# VxWorks target modifies configuration, only C-opt provider is allowed
############################################################################
if test "$TSPTARGET_VXWORKS" = "yes"; then
AC_MSG_CHECKING([for VxWorks TSP target config])
AC_MSG_WARN([Disabling TSP CONSUMERs and BB parts])
ENABLED_TSP_TARGET=vxworks
TSPCONSUMER="no"
GTK_BASED_CONSUMER="no"
BB="no"
TSPBUILD="opt"
fi


############################################################################
# Generate list of compilation
############################################################################
# define TSPBASE
TSP_BASE=`pwd`

list_targets=`echo $ENABLED_TSP_TARGET | sed -e "s/\:/ /g" `
list_modes=`echo $TSPBUILD | sed -e "s/\:/ /g" `
dest=$TSP_BASE/make/Makebuild.list
echo "Generating list of targets and modes build in $dest"


echo "# Generated from configure " > $dest

echo "all::" >> $dest
for target in $list_targets; do
	for mode in $list_modes; do
		echo "	\$(MAKE) TSP_TARGET=$target TSP_MODE=$mode		build" >> $dest
	done
done

echo "clean::" >> $dest
for target in $list_targets; do
	for mode in $list_modes; do
		echo "	\$(MAKE) TSP_TARGET=$target TSP_MODE=$mode		destroy" >> $dest
	done
done

echo "install::" >> $dest
for target in $list_targets; do
	for mode in $list_modes; do
		echo "	\$(MAKE) TSP_TARGET=$target TSP_MODE=$mode		install_once" >> $dest
	done
done


############################################################################

# 
#  Substitute Shell VARIABLES to OUTPUT VARIABLES
AC_SUBST(HOST_TARGET)

AC_SUBST(CC)
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)

AC_SUBST(CXX)
AC_SUBST(CXXFLAGS)

AC_SUBST(AR)
AC_SUBST(ARFLAGS)

AC_SUBST(LEX)
AC_SUBST(YACC)

AC_SUBST(LDLIBS)

AC_SUBST(TSP_BASE)
AC_SUBST(ENABLED_TSP_TARGET)
AC_SUBST(BB)
AC_SUBST(SHAREDLIB_ENABLED)
AC_SUBST(GL_OK)
AC_SUBST(GLUT_OK)
AC_SUBST(SCRIPT_BINDING)
AC_SUBST(TSPCONSUMER)
AC_SUBST(XML2_CONFIG_USER)
AC_SUBST(XMLWRITER_SUPPORTED)
AC_SUBST(GTK_BASED_CONSUMER)
AC_SUBST(LEX_YACC_PARSER)
AC_SUBST(TSPPROVIDER)
AC_SUBST(BUILD_XMLRPC)

# Config for outputed files
AC_CONFIG_FILES([src/scripts/tsp_dev.login
		src/scripts/tsp_dev.login.sh
		make/Makeparams
		src/core/include/tsp_prjcfg.h])

AC_OUTPUT

echo "***********************************************************"
echo "*  TSP has been successfully configured.                   "
echo "*  you should now do either                                "
echo "*     source src/scripts/tsp_dev.login  [[csh/tcsh]]       "
echo "*     . src/scripts/tsp_dev.login.sh    [[sh/bash/zsh/...]]" 
echo "*  And then you may build with a simple                    "
echo "*     make                                                 "
echo "*                                                          "
echo "*  Enjoy TSP !!!                                           "
echo "*  The TSP Team -- AC_PACKAGE_BUGREPORT                    "
echo "***********************************************************"
