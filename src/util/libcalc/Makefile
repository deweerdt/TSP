include $(DEVBASE)/make/Makeparams

# -----------------------------------------------------------
XINC    = calc_func.h

SOURCES = \
	calc_func.c 

OBJECTS=$(SOURCES:%.c=%.o) 

TARGET=$(DIRLIB)/libtsp_calc.a

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	 $(TARGET) 

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules



