# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGEDIR=tsp/consumer/jsynoptic
JARBASE=$(JARDIR)/jSynoptic/jsynoptic.jar:$(JARDIR)/jSynoptic/jfreechart.jar:$(JARDIR)/jSynoptic/simtools.jar:$(JARDIR)/jSynoptic/jcommon.jar

SOURCES = \
	TspSampleSymbolInfo.java \
	TspSymbolListModel.java \
	TspHandler.java

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

