include $(DEVBASE)/make/Makeparams


SUBDIRS = core util providers consumers


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
	- mkdir -p $(DIRSCRIPT)
	- cd $(DEVBASE)/exec; rm -f current; ln -s DEV current; cd $(DEVBASE)/src;

destroy::
	find . -name .make.state -exec rm -f {} \; -print
	$(RM) -r $(DIRBASE).old
	if [ -d $(DIRBASE) ] ; then \
		mv -i $(DIRBASE) $(DIRBASE).old ; \
	fi


include $(DEVBASE)/make/Makesubdirs

