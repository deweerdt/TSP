include $(DEVBASE)/make/Makeparams

XINC =	strings.h inttypes.h malloc.h libgen.h \
	sys/param.h sys/time.h sys/ipc.h sys/shm.h sys/sem.h sys/msg.h

build::
	mkdir -p $(DIRINC)/sys
	$(MAKE) export_include

destroy::
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules
