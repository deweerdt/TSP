#!/bin/sh

# show up script version
version ()
{
    printf "\n%s\n" "TSP Install Version script[sh] - V1.0"
}

cd $HOME_EXEC_BASE

version

if [ -z "$1" ]; then
    availv=`ls -d V-* DEV 2> /dev/null`
    echo "Available versions are:"
    for version in $availv
    do
	name="$version"
	if [ -L "$name" ]; then 
	    symlink=`ls -l $name | cut -d\> -f2-` 
	    local="(symbolic link to $symlink)"
	else
	    local=" "
        fi
	echo "  ---> $name $local" 
    done
    if [ -r current ]; then    
	current=`ls -l current | cut -d\> -f2-`
    else
	current="<None>"
    fi
    echo "Current installed version is: $current"
    echo "Use 'install_version <num_version>' to install available version <num_version>" 
    echo "<num_version> could be DEV"
    exit 1 
fi

if [ "$1" = "DEV" ]; then
    export DDEST=DEV
else
    export DDEST=V-$1
fi

if [ -L $DDEST ]; then
    target=`ls -l $DDEST | cut -d\> -f2-`
    if [ ! -d $target ]; then
	echo "ERROR : version <$DDEST> is a symbolic to <$target>, which is unreachable (networked version??)"
	exit 1
    fi
fi 

if [ ! -d $DDEST ]; then
    echo "ERROR : version $DDEST does not exist"
    exit 1
fi

# should remove an eventual existing link
# since GNU ln would create a link 'inside' current if
# not removed
\rm -f current
\ln -sf $DDEST current
echo "Post-installing Version $DDEST..."

echo "$HOME_EXEC_BASE is ready for use in $DDEST"


