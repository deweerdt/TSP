# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp/consumer/jsynoptic
JARBASE=$(JARDIR)/jSynoptic/jsynoptic.jar:$(JARDIR)/jSynoptic/jfreechart.jar:$(JARDIR)/jSynoptic/jcommon.jar

PLUGIN_JAR=$(DIRBASE)/java/jsynoptic-tsp-plugin.jar
ONCRPC_JAR=$(DEVBASE)/external/RemoteTea/oncrpc.jar

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
#	@echo $(JPACKAGEDIR)
	@cd $(CLASSBASE) ; jar cf $(PLUGIN_JAR) tsp/core tsp/util tsp/consumer/jsynoptic
	@echo ""
	@echo ----------------
	@echo Done! Use the script in this directory for testing.
	@echo Copy $(PLUGIN_JAR) and $(ONCRPC_JAR) to JSynoptic plugin directory '(ex: ~/.jsynoptic)' for standalone installation.
	@echo ----------------
	@echo ""

debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

destroy::
	$(MAKE) clean

include $(DEVBASE)/make/Makerules.java


