include $(DEVBASE)/make/Makeparams


ifeq (${BUILD_JAVA},yes)
	SUBDIRS = core jcore util providers consumers
else
	SUBDIRS = core util providers consumers
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
	- mkdir -p $(DIRSCRIPT)
	- mkdir -p $(CLASSBASE)
	- mkdir -p $(JARBASE)

destroy::
	find . -name .make.state -exec rm -f {} \; -print
	$(RM) -r $(DIRBASE).old
	if [ -d $(DIRBASE) ] ; then \
		mv -i $(DIRBASE) $(DIRBASE).old ; \
	fi


include $(DEVBASE)/make/Makesubdirs

