include $(DEVBASE)/make/Makeparams

#---------------------------------
# by default compile only src code
#---------------------------------
SUBDIRS = src

#-----------------------------------------------------------
# compile external first for VXWORKS target (posix + rpcgen)
#-----------------------------------------------------------
ifeq (${TSP_TARGET},vxworks)
SUBDIRS = external src
endif

#-----------------------------------------------
# compile external first for DEC target (rpcgen)
#-----------------------------------------------
ifeq (${TSP_TARGET},dec)
SUBDIRS = external src
endif

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build::
	- mkdir -p $(DIRBIN)
	- chmod a+ws $(DIRBIN)
	- mkdir -p $(DIRINC)
	- mkdir -p $(DIRLIB)
	- mkdir -p $(DIROBJ)
	- mkdir -p $(DIRSHLIB)
	- mkdir -p $(DIRSHOBJ)
	- mkdir -p $(DIRSCRIPT)
	- cd $(DEVBASE)/exec; rm -f current; ln -s DEV current; cd $(DEVBASE);

destroy::
	find . -name .make.state -exec rm -f {} \; -print
	$(RM) -r $(DIRBASE).old
	if [ -d $(DIRBASE) ] ; then \
		mv -i $(DIRBASE) $(DIRBASE).old ; \
	fi


include $(DEVBASE)/make/Makesubdirs

