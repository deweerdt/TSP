include $(DEVBASE)/make/Makeparams

# ---------------------
# Vars to build project
# ---------------------
TARGET=$(DIRBIN)/tsp_gdisp

# ---------------------
# How to build
# ---------------------

build::$(TARGET)

$(TARGET) : $(TSP_TARGET)/tsp_gdisp
	cp $(TSP_TARGET)/tsp_gdisp $(TARGET)

destroy::
	$(RM)	$(TARGET) 

include $(DEVBASE)/make/Makerules

