# -*- makefile -*-
# Makerules

# default targets section
# -----------------------

clean::
	@echo "@@@ removing class files ..."
	$(RM) *~
	$(RM) -rf $(CLASSBASE)/$(JPACKAGEDIR)

# Building rules section
# ----------------------

%.class : %.java
	@echo "@@@ Compiling $< (target $@)..."
	$(JAVAC) $(JFLAGS) -classpath $(CLASSBASE):$(JARBASE):$(JRPCJAR)  $< 

$(CLASSBASE)/$(JPACKAGEDIR)/%.class : %.java
	@echo "@@@ Compiling $< (target $@)..."
	$(JAVAC) $(JFLAGS) -classpath $(CLASSBASE):$(JARBASE):$(JRPCJAR)  -d $(CLASSBASE) $< 


%.jar : %.class
	$(JAR) $(ARFLAGS) $@ $% 


include $(DEVBASE)/make/Makerules.jrpc
