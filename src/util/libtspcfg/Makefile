include $(DEVBASE)/make/Makeparams

# -----------------------------------------------------------
XINC    = tspcfg_file.h

SOURCES = \
	tspcfg_file.c

OBJECTS=$(SOURCES:%.c=$(DIROBJ)/%.o)
TARGET=$(DIRLIB)/libtsp_cfg.a
TARGET_MAIN=$(DIRBIN)/tsp_cfg

CPPFLAGS += `xml2-config --cflags`
ADD_LIB += $(DIRLIB)/libtsp_common.a $(DIRLIB)/libtsp_cfg.a
LDLIBS += -ltsp_common -ltsp_cfg `xml2-config --libs` 

# -----------------------------------------------------------
# No label all:: because it's in Makeparams and calls build::	
# same thing for clean:: that calls destroy::
# -----------------------------------------------------------

build:: $(TARGET)($(OBJECTS)) $(TARGET_MAIN)
	$(MAKE) export_include

$(TARGET_MAIN): $(DIRLIB)/libtsp_cfg.a $(DIROBJ)/tspcfg_file_main.o
	@echo "@@@ Specific Linking  $(TARGET2) ..."
	$(LINK.c) -o $(TARGET_MAIN) $(DIROBJ)/tspcfg_file_main.o -L$(DIRLIB) $(ADD_LIB) $(LDLIBS) 

destroy::
	$(RM) 	 $(TARGET) $(SHOBJECTS)
	$(MAKE) remove_include	

# -----------------------------------------------------------
include $(DEVBASE)/make/Makerules


