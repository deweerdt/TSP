# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp.core.provider
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

SOURCES = \

OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

build::
	$(MAKE) common debug

common:: $(OBJECTS)
	@echo $(JPACKAGEDIR)
debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

include $(DEVBASE)/make/Makerules.java
