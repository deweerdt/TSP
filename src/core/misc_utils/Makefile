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

clean::
	$(MAKE) 	destroy

debug::
	$(MAKE) D=.debug	common

build : $(TARGET)($(OBJECTS))


destroy :
	$(RM) 	 $(TARGET) 

print:
	@for file in Makefile $(INCLUDES) global_type.x DESC.GLOBAL $(SOURCES); \
	 do \
		echo "printing $$file..."; \
		il $$file; \
	 done


include $(DEVBASE)/make/Makerules
