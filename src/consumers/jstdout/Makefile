# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp.consumer
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

SOURCES = \
	jtspStdOut.java 

ifeq (${BUILD_JAVA},yes)
    OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)
endif

all::
	$(MAKE) build

build:: $(OBJECTS)
	@echo $(JPACKAGEDIR)
debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

include $(DEVBASE)/make/Makerules.java

