#!/bin/sh

echo "**********************************************************"
echo "           configuration for TSP compilation"
echo "**********************************************************"
echo "----------------------------------------------------------"
echo "(FIXME 1 : This is NOT a real Autoconf/Automake configure )"
echo "----------------------------------------------------------"

PWDPATH=`pwd`
WHERE="${PWDPATH}/src/scripts"
GENSCRIPT="${WHERE}/tsp_dev.login.sh"
TEMPLATE="${WHERE}/.templ_tsp_dev.login.sh"
SEDEXP="s:%%tsp_base%%:${PWDPATH}:"

sed -e "$SEDEXP" < $TEMPLATE > $GENSCRIPT
chmod a+x $GENSCRIPT
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





