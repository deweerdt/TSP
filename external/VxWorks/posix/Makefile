# This file contains the makefile rules for building the posix library

include $(DEVBASE)/make/Makeparams

SOURCES = \
	posixIpc.c \
	posixThread.c \
	posixWrap.c

TARGET=$(DIRLIB)/libtsp_posix.a

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o)

generate: $(TARGET)($(OBJECTS))

build::
	cd h; $(MAKE); cd .. 
	$(MAKE) generate

destroy::
	cd h; $(MAKE) clean; cd ..
	$(RM)    $(TARGET)

include $(DEVBASE)/make/Makerules
