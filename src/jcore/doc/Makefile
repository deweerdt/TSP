# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))
JDOCDIR=$(DEVBASE)/doc/jcore

SOURCESPATH=$(DEVBASE)/src/jcore

OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

all::
	$(MAKE) doc

doc::
	javadoc -d $(JDOCDIR) \
	-subpackages $(JPACKAGE) \
	-sourcepath $(SOURCESPATH) \
	-link file:$(DEVBASE)/../RemoteTea/doc

destroy::
	$(MAKE) clean

clean::
	$(RM) -rf $(JDOCDIR)

include $(DEVBASE)/make/Makerules.java
