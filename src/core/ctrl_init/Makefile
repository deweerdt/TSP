include $(DEVBASE)/make/Makeparams.$(T)

XINC    = tsp_provider_init.h

SOURCES = \
	tsp_provider_init.c

OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_provider.a

.PRECIOUS: $(TARGET)

all::
	$(MAKE) common debug

clean::
	$(MAKE) destroy
common:
	$(MAKE) T=provider$(D) build
debug::
	$(MAKE) D=.debug	common

build : $(TARGET)($(OBJECTS))



destroy :
	$(RM) 	$(TARGET) 




include $(DEVBASE)/make/Makerules
