
include $(DEVBASE)/make/Makeparams.$(T)

RPC=NONE

XINC    = libUTIL.h


SOURCES = \
	alias.c \
	use_dbl.c \
	nl.c \
	initsim.c \
	outp_i.c \
	outp_1.c \
	sequenc.c \
	datar.c \
	mdatar.c \
	dataw.c \
	dataf.c \
	tokex.c \
	unit.c \
	var_adres.c \
	iadr.c



OBJECTS=$(SOURCES:%.c=%.o) 


TARGET=$(DIRLIB)/libtsp_res.a

.PRECIOUS: $(TARGET)

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



