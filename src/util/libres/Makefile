include $(DEVBASE)/make/Makeparams

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


# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	 $(TARGET) $(OBJECTS)

include $(DEVBASE)/make/Makerules



