include $(DEVBASE)/make/Makeparams.$(T)

XINC    = tsp_provider_init.h

SOURCES = \
	tsp_provider_init.c

OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_provider_init.a

.PRECIOUS: $(TARGET)

all::
	$(MAKE) common debug

common:
	$(MAKE) T=provider$(D) build
debug::
	$(MAKE) D=.debug	common

build : $(TARGET)($(OBJECTS))

#------------------------------------

clean::
	$(MAKE) clean_common clean_debug

clean_debug:
	$(MAKE) D=.debug clean_common

clean_common:
	$(MAKE) T=provider$(D) destroy

destroy :
	$(RM) 	$(TARGET)


include $(DEVBASE)/make/Makerules
