# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams

JPACKAGE=tsp.core.config
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

SOURCES = \
	TspConfig.java 

OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

all::
	$(MAKE) common debug

common:: $(OBJECTS)
	@echo $(JPACKAGEDIR)
debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

include $(DEVBASE)/make/Makerules.java
