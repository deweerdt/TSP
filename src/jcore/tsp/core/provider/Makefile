# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams

JPACKAGE=tsp.core.provider
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

SOURCES = \

OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

all::
	$(MAKE) common debug

common:: $(OBJECTS)
	@echo $(JPACKAGEDIR)
debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

include $(DEVBASE)/make/Makerules.java
