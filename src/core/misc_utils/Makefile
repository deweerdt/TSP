include $(DEVBASE)/make/Makeparams.$(T)

XINC    = tsp_time.h tsp_ringbuf.h

XSCRIPT = 


SOURCES = \
	tsp_time.c 



OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_services.a

.PRECIOUS: $(TARGET)

all::
	$(MAKE) common debug
common:
	$(MAKE) T=consumer$(D)	build
	$(MAKE) T=provider$(D)	build

debug::
	$(MAKE) D=.debug	common


build : $(TARGET)($(OBJECTS))


#----------------------------------

clean::
	$(MAKE) clean_common clean_debug

clean_debug:
	$(MAKE) D=.debug clean_common

clean_common:
	$(MAKE) T=provider$(D) destroy
	$(MAKE) T=consumer$(D) destroy

destroy :
	$(RM) 	$(TARGET)




include $(DEVBASE)/make/Makerules
