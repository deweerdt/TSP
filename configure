#!/bin/csh

echo "**********************************************************"
echo "           configuration for TSP compilation"
echo "**********************************************************"
echo "----------------------------------------------------------"
echo "(FIXME 1 : This is NOT a real Autoconf/Automake configure )"
echo "(FIXME 2 : All scripts need a tcsh or csh shell )"
echo "----------------------------------------------------------"

set PWDPATH = `pwd`
set WHERE = "${PWDPATH}/src/scripts"
set GENSCRIPT = "${WHERE}/tsp_dev.login"
set TEMPLATE = "${WHERE}/.templ_tsp_dev.login"
set SEDEXP = "s:%%tsp_base%%:${PWDPATH}:"

sed -e $SEDEXP < $TEMPLATE > $GENSCRIPT

echo "${GENSCRIPT} was created"
echo ""
echo "Now you should type the following commands : "
echo ""
echo "% tcsh"
echo "% source ${GENSCRIPT}"
echo "% cd src"
echo "% make"
echo "% rehash"
echo "% install_version DEV"
echo "% tsp_autotest001"
echo ""
echo "The latest command launch some autotest to check that"
echo "everything's all right."
echo ""
