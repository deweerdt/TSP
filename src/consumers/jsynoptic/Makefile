# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp/consumer/jsynoptic
JARBASE=$(JARDIR)/jSynoptic/jsynoptic.jar:$(JARDIR)/jSynoptic/jfreechart.jar:$(JARDIR)/jSynoptic/simtools.jar:$(JARDIR)/jSynoptic/jcommon.jar

SUBDIRS = \
	impl \
	ui

SOURCES = \
	TspDataSourceCollection.java \
	TspDataPlugin.java

OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

include $(DEVBASE)/make/Makesubdirs

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


