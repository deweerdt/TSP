include $(DEVBASE)/make/Makeparams.$(T)

# ---------------------
# Vars to build project
# ---------------------
TARGET=$(DIRBIN)/tsp_gdisp

# ---------------------
# How to build
# ---------------------
all::
	$(MAKE) T=consumer	build 

build : $(TARGET)

$(TARGET) : $(HOST_TARGET)/tsp_gdisp
	cp $(HOST_TARGET)/tsp_gdisp $(TARGET)

clean::
	$(MAKE) T=consumer	destroy

destroy :
	$(RM)	$(TARGET) 

include $(DEVBASE)/make/Makerules

