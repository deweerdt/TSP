include $(DEVBASE)/make/Makeparams

XINC    = tsp_common.h \
	tsp_common_request.h \
	tsp_common_filter.h \
	tsp_common_ssi.h \
	tsp_common_ssei.h \
	tsp_common_as.h \
	tsp_decoder.h \
	tsp_encoder.h
SOURCES = \
	tsp_common.c \
	tsp_common_request.c \
	tsp_common_ssi.c \
	tsp_common_ssei.c \
	tsp_common_as.c \
	tsp_encoder.c \
	tsp_decoder.c

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o) 

TARGET=$(DIRLIB)/libtsp_common.a

CPPFLAGS += -I. #./rpc -I../misc_utils -I../driver -I../ctrl -I../include 

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: export_include $(TARGET)($(OBJECTS))

destroy::
	$(RM) 	$(TARGET)
	$(MAKE) remove_include

include $(DEVBASE)/make/Makerules
