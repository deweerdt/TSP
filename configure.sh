#!/bin/sh

echo "**********************************************************"
echo "           configuration for TSP compilation"
echo "**********************************************************"
echo "----------------------------------------------------------"
echo "(FIXME 1 : This is NOT a real Autoconf/Automake configure )"
echo "----------------------------------------------------------"

PWDPATH=`pwd`
GENSCRIPT="${PWDPATH}/src/scripts/tsp_gen_dev.login.sh"
TEMPLATE="${GENSCRIPT}.templ"
SEDEXP="s:%%tsp_base%%:${PWDPATH}:"

sed -e "$SEDEXP" < $TEMPLATE > $GENSCRIPT

echo "${GENSCRIPT} was created"
echo ""
echo "Now you should type the following commands : "
echo ""
echo "% . ${GENSCRIPT}"
echo "% cd src"
echo "% make"
echo "% install_version.sh DEV"
echo "% tsp_autotest001"
echo ""
echo "The latest command launch some autotest to check that"
echo "everything's all right."
echo ""
