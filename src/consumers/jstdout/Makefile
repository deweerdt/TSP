# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp.consumer
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

SOURCES = \
	jtspStdOut.java 


OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

all::
	$(MAKE) build

build:: $(OBJECTS)
	@echo $(JPACKAGEDIR)
debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

destroy::
	$(MAKE) clean

include $(DEVBASE)/make/Makerules.java

