# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp.core.consumer
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

SOURCES = \
	TspSession.java \
	TspConsumer.java 


OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

build::
	$(MAKE) common debug

common:: $(OBJECTS)
	@echo $(JPACKAGEDIR)
debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

include $(DEVBASE)/make/Makerules.java
