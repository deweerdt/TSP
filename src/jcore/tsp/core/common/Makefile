# -*- Makefile -*-

include $(DEVBASE)/make/Makeparams.java

JPACKAGE=tsp.core.common
JPACKAGEDIR=$(subst .,/,$(JPACKAGE))

SOURCES = \
	TspException.java \
	TspAnswer.java \
	TspAnswerOpen.java \
	TspAnswerSample.java \
	TspAnswerSampleInit.java \
	TspAnswerSampleFinalize.java \
	TspRequest.java \
	TspRequestOpen.java \
	TspRequestClose.java \
	TspRequestInfos.java \
	TspRequestSample.java \
	TspRequestSampleInit.java \
	TspRequestSampleFinalize.java \
	TspSampleSymbols.java \
	TspGroupItem.java \
	TspGroup.java \
	TspSample.java \
	TspSampleFifo.java \
	TspDecodingStream.java \
	TspDataInputStream.java \
	TspStreamReader.java 

OBJECTS=$(SOURCES:%.java=$(CLASSBASE)/$(JPACKAGEDIR)/%.class)

all::
	$(MAKE) common debug

common:: $(OBJECTS)
	@echo $(JPACKAGEDIR)
debug::
	@echo "No debug for yet"
#	$(MAKE) D=.debug common

destroy::
	$(MAKE) clean

include $(DEVBASE)/make/Makerules.java
