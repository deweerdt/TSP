include $(DEVBASE)/make/Makeproject
all::
	$(MAKE) dirs

dirs:
	- mkdir -p $(BINBASE)
	- chmod a+ws $(BINBASE)
	- chgrp essai $(BINBASE)
	- mkdir -p $(DIRINC)
	- mkdir -p $(LIBBASE)
	- mkdir -p $(LIBBASE)/lib.consumer
	- mkdir -p $(LIBBASE)/lib.consumer.debug
	- mkdir -p $(LIBBASE)/lib.provider
	- mkdir -p $(LIBBASE)/lib.provider.debug
	- mkdir -p $(DIRSCRIPT)
	- mkdir -p $(BINBASE)/bin.consumer
	- mkdir -p $(BINBASE)/bin.consumer.debug
	- mkdir -p $(BINBASE)/bin.provider
	- mkdir -p $(BINBASE)/bin.provider.debug

clean::
	find . -name .make.state -exec rm -f {} \; -print
	$(RM) -r $(BINBASE).old
	mv -i $(BINBASE) $(BINBASE).old

SUBDIRS =  \
	tsp  

include $(DEVBASE)/make/Makesubdirs
