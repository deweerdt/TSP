# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGEDIR=tsp/consumer/jsynoptic/ui/ressources

SOURCES = \
	tsp_pieuvre3.png

OBJECTS=$(SOURCES:%.png=$(CLASSBASE)/$(JPACKAGEDIR)/%.png)

all::
	$(MAKE) build

build:: $(OBJECTS)
	@echo "DONE ===" $(JPACKAGEDIR) " ==="

destroy::
	$(MAKE) clean

$(CLASSBASE)/$(JPACKAGEDIR)/%.png : %.png
	@echo "@@@ Copying $< (target $@)..."
	mkdir -p $(CLASSBASE)/$(JPACKAGEDIR)/
	cp $< $@

clean::
	@echo "@@@ removing ressource files ..."
	$(RM) *~
	$(RM) -rf $(CLASSBASE)/$(JPACKAGEDIR)
