
include $(DEVBASE)/make/Makeparams.$(T)

XINC    = calc_func.h

SOURCES = \
	calc_func.c 



OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_calc.a

all::
	$(MAKE) common debug
common:
	$(MAKE) T=consumer$(D)	build
	$(MAKE) T=provider$(D)	build	

debug::
	$(MAKE) D=.debug	common

build : $(TARGET)($(OBJECTS))




clean::
	$(MAKE) clean_common clean_debug

clean_debug:
	$(MAKE) D=.debug clean_common

clean_common:
	$(MAKE) T=provider$(D) destroy

destroy :
	$(RM) 	 $(TARGET) 





include $(DEVBASE)/make/Makerules



