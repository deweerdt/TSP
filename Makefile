include $(DEVBASE)/make/Makeparams

#---------------------------------
# by default compile only src code
#---------------------------------
SUBDIRS = external src

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

distclean: destroy
	if [ -d $(DEVBASE)/dist ] ; then \
	   $(RM) -r $(DEVBASE)/dist; \
	fi

destroy::
	find . -name .make.state -exec rm -f {} \; -print
	$(RM) -r $(DIRBASE).old
	if [ -d $(DIRBASE) ] ; then \
		mv -i $(DIRBASE) $(DIRBASE).old ; \
	fi

install_once::
	$(INSTALL_DATA) -D $(DEVBASE)/exec/current/include/tsp_consumer.h $(prefix)/include/tsp_consumer.h
	$(INSTALL_DATA)  $(DEVBASE)/exec/current/include/* $(prefix)/include
	$(INSTALL_DATA) -D $(DEVBASE)/exec/current/$(TSP_TARGET)/$(TSP_MODE)/lib/libtsp_consumer.a $(prefix)/lib/libtsp_consumer.a
	$(INSTALL_DATA)  $(DEVBASE)/exec/current/$(TSP_TARGET)/$(TSP_MODE)/lib/* $(prefix)/lib
	$(INSTALL_PROGRAM) -D $(DEVBASE)/exec/current/$(TSP_TARGET)/$(TSP_MODE)/bin/tsp_stub_server $(exec_prefix)/bin/tsp_stub_server
	$(INSTALL_PROGRAM) $(DEVBASE)/exec/current/$(TSP_TARGET)/$(TSP_MODE)/bin/* $(exec_prefix)/bin
	$(INSTALL_PROGRAM) -D $(DEVBASE)/exec/current/scripts/bb_check_id $(exec_prefix)/scripts/bb_check_id
	$(INSTALL_PROGRAM) $(DEVBASE)/exec/current/scripts/* $(exec_prefix)/scripts
	$(INSTALL_PROGRAM) $(DEVBASE)/src/scripts/tsp_profile.sh $(DEVBASE)/src/scripts/tsp_profile.csh $(exec_prefix)/scripts
	$(INSTALL_PROGRAM) $(DEVBASE)/src/scripts/psinfo.sh $(exec_prefix)/scripts
	$(INSTALL_DATA) $(DEVBASE)/src/scripts/tsp.pc $(exec_prefix)/scripts

ifeq (${APIDOC_GEN},yes)
	mkdir -p $(exec_prefix)/doc/api/html/footer
	mkdir -p $(exec_prefix)/doc/api/html/header
	$(INSTALL_DATA) $(DEVBASE)/dist/doc/api/html/*.* $(exec_prefix)/doc/api/html
	$(INSTALL_DATA) $(DEVBASE)/dist/doc/api/html/footer/*.html $(exec_prefix)/doc/api/html/footer
	$(INSTALL_DATA) $(DEVBASE)/dist/doc/api/html/header/*.html $(exec_prefix)/doc/api/html/header
else
	mkdir -p $(exec_prefix)/doc/api/html/footer
	mkdir -p $(exec_prefix)/doc/api/html/header
	$(INSTALL_DATA) $(DEVBASE)/src/doxy/footer/*.html $(exec_prefix)/doc/api/html/footer
	$(INSTALL_DATA) $(DEVBASE)/src/doxy/header/*.html $(exec_prefix)/doc/api/html/header
endif

include $(DEVBASE)/make/Makesubdirs

